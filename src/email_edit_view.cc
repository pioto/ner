/* ner: src/email_edit_view.cc
 *
 * Copyright (c) 2010 Michael Forney
 * Copyright (c) 2011 Maxime Coste
 *
 * This file is a part of ner.
 *
 * ner is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3, as published by the Free
 * Software Foundation.
 *
 * ner is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ner.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <sys/time.h>
#include <gio/gio.h>
#include <gmime/gmime.h>

#include "email_edit_view.hh"
#include "view_manager.hh"
#include "maildir.hh"
#include "ner_config.hh"
#include "util.hh"

EmailEditView::EmailEditView(const View::Geometry & geometry)
    : EmailView(geometry),
        _identity(IdentityManager::instance().defaultIdentity())
{
    setVisibleHeaders(std::vector<std::string>{
        "From",
        "To",
        "Cc",
        "Bcc",
        "Subject"
    });

    /* Key Sequences */
    addHandledSequence("e", std::bind(&EmailEditView::edit, this));
    addHandledSequence("a", std::bind(&EmailEditView::attach, this));
    addHandledSequence("d", std::bind(&EmailEditView::removeSelectedAttachment, this));
    addHandledSequence("y", std::bind(&EmailEditView::send, this));
    addHandledSequence("f", std::bind(&EmailEditView::toggleSelectedPartFolding, this));
}

EmailEditView::~EmailEditView()
{
}

void EmailEditView::edit()
{
    endwin();

    std::string command(NerConfig::instance().command("edit"));
    command.push_back(' ');
    command.append(_messageFile);
    std::system(command.c_str());

    PartList partsBackup;
    partsBackup.swap(_parts); // parts will be cleared anyway

    setEmail(_messageFile);

    std::copy_if(partsBackup.begin(), partsBackup.end(),
                 std::back_inserter(_parts),
                 [] (std::shared_ptr<MessagePart>& part) -> bool { return dynamic_cast<Attachment*>(part.get()); });
}

void EmailEditView::createMessage(GMimeMessage * message)
{
    char * temporaryFilePath = strdup("/tmp/ner-compose-XXXXXX");
    int fd = mkstemp(temporaryFilePath);
    _messageFile = temporaryFilePath;
    free(temporaryFilePath);

    GMimeStream * stream = g_mime_stream_fs_new(fd);
    g_mime_object_write_to_stream(GMIME_OBJECT(message), stream);

    g_object_unref(stream);
    g_object_unref(message);
}

void EmailEditView::send()
{
    /* Add the date to the message */
    FILE * file = fopen(_messageFile.c_str(), "r");
    GMimeStream * stream = g_mime_stream_file_new(file);
    GMimeParser * parser = g_mime_parser_new_with_stream(stream);
    GMimeMessage * message = g_mime_parser_construct_message(parser);
    g_object_unref(parser);
    g_object_unref(stream);

    struct timeval timeValue;
    struct timezone timeZone;

    gettimeofday(&timeValue, &timeZone);

    g_mime_message_set_date(message, timeValue.tv_sec, -100 * timeZone.tz_minuteswest / 60);

    /* Give the message an ID */
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    std::ostringstream messageId;
    messageId << timeValue.tv_sec << '-' << "ner" << '-' << rand() % 10000 << '@' << hostname;

    g_mime_message_set_message_id(message, messageId.str().c_str());

    if (_parts.size() > 1)
    {
        GMimeMultipart* multipart = g_mime_multipart_new_with_subtype("mixed");
        g_mime_multipart_add(multipart, (GMimeObject*)message->mime_part);
        g_mime_message_set_mime_part(message, (GMimeObject*) multipart);

        for (auto i = _parts.begin(); i != _parts.end(); ++i)
        {
            if (not dynamic_cast<Attachment*>(i->get()))
                continue;

            Attachment& attachment = *dynamic_cast<Attachment*>(i->get());

            GMimeContentType* contentType = g_mime_content_type_new_from_string(attachment.contentType.c_str());

            GMimePart* part = g_mime_part_new_with_type(g_mime_content_type_get_media_type(contentType),
                                                        g_mime_content_type_get_media_subtype(contentType));
            g_mime_part_set_content_object(part, attachment.data);
            g_mime_part_set_content_encoding(part, GMIME_CONTENT_ENCODING_BASE64);
            g_mime_part_set_filename(part, attachment.filename.c_str());

            g_mime_multipart_add(multipart, (GMimeObject*) part);
            g_object_unref(part);
            g_object_unref(contentType);
        }
        g_object_unref(multipart);
    }

    /* Send the message */
    std::string sendCommand = _identity->sendCommand.empty() ?
        NerConfig::instance().command("send") : _identity->sendCommand;
    FILE * sendMailPipe = popen(sendCommand.c_str(), "w");
    GMimeStream * sendMailStream = g_mime_stream_file_new(sendMailPipe);
    g_mime_stream_file_set_owner(GMIME_STREAM_FILE(sendMailStream), false);
    g_mime_object_write_to_stream(GMIME_OBJECT(message), sendMailStream);
    g_object_unref(sendMailStream);

    int status = pclose(sendMailPipe);

    if (status == 0)
    {
        StatusBar::instance().displayMessage("Message sent successfully");

        if (_identity->sentMail)
            if (!_identity->sentMail->addMessage(message))
                StatusBar::instance().displayMessage("Could not add message to configured mail store");

        unlink(_messageFile.c_str());
        ViewManager::instance().closeActiveView();
    }
    else
        StatusBar::instance().displayMessage("Could not send the message");

    g_object_unref(message);
}

void EmailEditView::attach()
{
    std::string filename = StatusBar::instance().prompt("Filename: ", "attachments");

    GError* error = NULL;
    auto file = autoUnref(g_file_new_for_path(filename.c_str()));
    auto fileinfo = autoUnref(g_file_query_info(file,
                                                G_FILE_ATTRIBUTE_STANDARD_TYPE ","  G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                                                G_FILE_QUERY_INFO_NONE, NULL, &error));
    if (error)
    {
        StatusBar::instance().displayMessage("Could not query file information");
        return;
    }
    if (g_file_info_get_file_type(fileinfo) != G_FILE_TYPE_REGULAR)
    {
        StatusBar::instance().displayMessage("File is not a regular file");
        return;
    }

    auto filestream = autoUnref(g_mime_stream_file_new(fopen(filename.c_str(), "r")));
    auto data = autoUnref(g_mime_data_wrapper_new_with_stream(filestream, GMIME_CONTENT_ENCODING_DEFAULT));

    _parts.push_back(std::make_shared<Attachment>(data, g_file_get_basename(file),
                                                  g_file_info_get_content_type(fileinfo),
                                                  g_mime_stream_length(filestream)));
}

void EmailEditView::removeSelectedAttachment()
{
    PartList::iterator selection = selectedPart();
    if (dynamic_cast<Attachment*>(selection->get()))
        _parts.erase(selection);
}

void EmailEditView::setIdentity(const std::string & name)
{
    const Identity * identity = IdentityManager::instance().findIdentity(name);

    if (identity)
        _identity = identity;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

