/* ner: src/reply_view.cc
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

#include <sstream>
#include <iterator>
#include <strings.h>

#include "reply_view.hh"
#include "notmuch.hh"
#include "util.hh"

ReplyView::ReplyView(const std::string & messageId, const View::Geometry & geometry)
    : EmailEditView(geometry)
{
    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_message_t * message = notmuch_database_find_message(database, messageId.c_str());

    if (!message)
    {
        notmuch_database_close(database);
        throw NotMuch::InvalidMessageException(messageId);
    }

    FILE * messageFile = fopen(notmuch_message_get_filename(message), "r");
    GMimeStream * stream = g_mime_stream_file_new(messageFile);
    GMimeParser * parser = g_mime_parser_new_with_stream(stream);

    notmuch_message_destroy(message);
    notmuch_database_close(database);

    GMimeMessage * originalMessage = g_mime_parser_construct_message(parser);
    GMimeMessage * replyMessage = g_mime_message_new(true);

    g_object_unref(parser);
    g_object_unref(stream);

    /* Set subject */
    std::string replyPrefix("Re:");
    std::string subject(g_mime_message_get_subject(originalMessage));
    if (!std::equal(replyPrefix.begin(), replyPrefix.end(), subject.begin()))
    {
        subject.insert(0, "Re: ");
    }

    g_mime_message_set_subject(replyMessage, subject.c_str());

    /* Set references */
    const char * originalReferences = g_mime_object_get_header(GMIME_OBJECT(originalMessage), "References");
    std::string references;
    std::string originalMessageId;
    originalMessageId.push_back('<');
    originalMessageId.append(g_mime_message_get_message_id(originalMessage));
    originalMessageId.push_back('>');

    if (originalReferences)
    {
        references.append(originalReferences);
        references.push_back(' ');
    }

    references.append(originalMessageId);

    g_mime_object_set_header(GMIME_OBJECT(replyMessage), "References", references.c_str());
    g_mime_object_set_header(GMIME_OBJECT(replyMessage), "In-Reply-To", originalMessageId.c_str());

    /* Set addresses */
    InternetAddress * userAddress = NULL;

    std::vector<GMimeRecipientType> recipientTypes{
        GMIME_RECIPIENT_TYPE_TO,
        GMIME_RECIPIENT_TYPE_CC,
        GMIME_RECIPIENT_TYPE_BCC
    };

    const char * userName = g_key_file_get_string(NotMuch::config(), "user", "name", NULL);
    const char * primaryAddress = g_key_file_get_string(NotMuch::config(), "user", "primary_email", NULL);
    size_t otherAddressesLength;
    char ** otherAddresses = g_key_file_get_string_list(NotMuch::config(), "user", "other_email",
        &otherAddressesLength, NULL);

    const char * sender = g_mime_object_get_header(GMIME_OBJECT(originalMessage), "From");
    InternetAddressList * senderAddressList = internet_address_list_parse_string(sender);
    InternetAddress * senderAddress = internet_address_list_get_address(senderAddressList, 0);
    internet_address_list_add(g_mime_message_get_recipients(replyMessage, GMIME_RECIPIENT_TYPE_TO), senderAddress);
    g_object_unref(senderAddressList);

    for (auto recipientType = recipientTypes.begin();
        recipientType != recipientTypes.end();
        ++recipientType)
    {
        InternetAddressList * addresses = g_mime_message_get_recipients(originalMessage, *recipientType);

        for (int index = 0; index < internet_address_list_length(addresses); ++index)
        {
            InternetAddress * address = internet_address_list_get_address(addresses, index);

            /* Check if this is the user address */
            if (strcasecmp(primaryAddress, internet_address_mailbox_get_addr(INTERNET_ADDRESS_MAILBOX(address))) == 0)
                /* We default to the primary anyway */
                continue;

            bool found = false;
            for (int otherIndex = 0; otherIndex < otherAddressesLength; ++otherIndex)
            {
                if (strcasecmp(otherAddresses[otherIndex], internet_address_mailbox_get_addr(INTERNET_ADDRESS_MAILBOX(address))) == 0)
                {
                    userAddress = internet_address_mailbox_new(
                        userName,
                        internet_address_mailbox_get_addr(INTERNET_ADDRESS_MAILBOX(address))
                    );
                    found = true;
                    break;
                }
            }

            /* Otherwise its just a normal recipient */
            if (!found)
                internet_address_list_add(g_mime_message_get_recipients(replyMessage, *recipientType), address);
        }
    }

    if (!userAddress)
         userAddress = internet_address_mailbox_new(userName, primaryAddress);

    g_mime_message_set_sender(replyMessage, internet_address_to_string(userAddress, true));

    /* Set content */
    std::ostringstream messageContentStream;
    messageContentStream << "On " << g_mime_message_get_date_as_string(originalMessage) << ", ";
    messageContentStream << g_mime_message_get_sender(originalMessage) << " wrote:" << std::endl << "> ";

    GMimeObject * part = g_mime_message_get_mime_part(originalMessage);
    mimePartLines(part, std::ostream_iterator<std::string>(messageContentStream, "\n> "));
    g_object_unref(part);

    std::string messageContent(messageContentStream.str());

    /* Get rid of the last '> ' */
    messageContent.resize(messageContent.size() - 3);
    messageContent.append("\n\n");

    GMimeStream * contentStream = g_mime_stream_mem_new_with_buffer(messageContent.c_str(), messageContent.size());
    GMimePart * replyPart = g_mime_part_new_with_type("text", "plain");
    GMimeDataWrapper * contentWrapper = g_mime_data_wrapper_new_with_stream(contentStream, GMIME_CONTENT_ENCODING_DEFAULT);
    g_mime_part_set_content_object(replyPart, contentWrapper);
    g_mime_message_set_mime_part(replyMessage, GMIME_OBJECT(replyPart));
    g_object_unref(replyPart);
    g_object_unref(contentWrapper);
    g_object_unref(contentStream);

    createMessage(replyMessage);

    edit();
}

ReplyView::~ReplyView()
{
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

