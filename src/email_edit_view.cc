/* ner: src/email_edit_view.cc
 *
 * Copyright (c) 2010 Michael Forney
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
#include <gmime/gmime.h>

#include "email_edit_view.hh"
#include "view_manager.hh"

const std::string editorCommand = "vim +";
const std::string sendCommand = "/usr/sbin/sendmail -t";

EmailEditView::EmailEditView(const View::Geometry & geometry)
    : EmailView(geometry)
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
    addHandledSequence("y", std::bind(&EmailEditView::send, this));
}

EmailEditView::~EmailEditView()
{
}

void EmailEditView::edit()
{
    endwin();

    std::string command(editorCommand);
    command.push_back(' ');
    command.append(_messageFile);
    std::system(command.c_str());

    setEmail(_messageFile);
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

    /* Send the message */
    FILE * sendMailPipe = popen(sendCommand.c_str(), "w");
    GMimeStream * sendMailStream = g_mime_stream_file_new(sendMailPipe);
    g_mime_stream_file_set_owner(GMIME_STREAM_FILE(sendMailStream), false);
    g_mime_object_write_to_stream(GMIME_OBJECT(message), sendMailStream);
    g_object_unref(sendMailStream);

    int status = pclose(sendMailPipe);

    if (status == 0)
    {
        StatusBar::instance().displayMessage("Message sent successfully");

        unlink(_messageFile.c_str());
        ViewManager::instance().closeActiveView();
    }
    else
        StatusBar::instance().displayMessage("Could not send the message");

    g_object_unref(message);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

