/* ner: src/message_view.cc
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

#include <fstream>
#include <sstream>
#include <cstring>

#include "message_view.hh"
#include "notmuch.hh"
#include "colors.hh"
#include "ncurses.hh"
#include "status_bar.hh"

const std::vector<std::string> headers{ "To", "From", "Subject" };

MessageView::MessageView(notmuch_message_t * message)
    : LineBrowserView()
{
    std::string filename = notmuch_message_get_filename(message);

    FILE * file = fopen(filename.c_str(), "r");

    if (file != NULL)
    {
        GMimeStream * stream = g_mime_stream_file_new(file);
        GMimeParser * parser = g_mime_parser_new_with_stream(stream);
        GMimeMessage * message = g_mime_parser_construct_message(parser);

        /* Read relavant headers */
        _headers["To"] = internet_address_list_to_string(g_mime_message_get_recipients(message,
            GMIME_RECIPIENT_TYPE_TO), true) ? : "(null)";
        _headers["From"] = g_mime_message_get_sender(message) ? : "(null)";
        _headers["Subject"] = g_mime_message_get_subject(message) ? : "(null)";

        GMimeObject * mimePart = g_mime_message_get_mime_part(message);

        /* Locate plain text parts */
        processMimePart(mimePart);
    }
    else
    {
        _headers["To"]      = "(null)";
        _headers["From"]    = "(null)";
        _headers["Subject"] = "(null)";
    }

    /* Colors */
    init_pair(Colors::MESSAGE_VIEW_HEADER,  COLOR_CYAN, COLOR_BLACK);
}

MessageView::~MessageView()
{
}

MessageView * MessageView::fromId(const std::string & messageId)
{
    MessageView * messageView;

    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_message_t * message = notmuch_database_find_message(database, messageId.c_str());

    if (message != NULL)
        messageView = new MessageView(message);
    else
    {
        messageView = 0;
        StatusBar::instance().displayMessage("Cannot find message with ID: " + messageId);
    }

    notmuch_database_close(database);

    return messageView;
}

void MessageView::update()
{
    int row = 0;

    werase(_window);

    for (auto header = headers.begin(), e = headers.end(); header != e; ++header, ++row)
    {
        int x = 0;

        wmove(_window, row, x);

        x += NCurses::addPlainString(_window, (*header) + ": ",
            0, Colors::MESSAGE_VIEW_HEADER);

        if (x >= getmaxx(_window))
        {
            NCurses::addCutOffIndicator(_window, 0);
            continue;
        }

        wmove(_window, row, x);

        x += NCurses::addUtf8String(_window, _headers[*header].c_str());

        if (x > getmaxx(_window))
            NCurses::addCutOffIndicator(_window, 0);
    }

    wmove(_window, row, 0);
    whline(_window, 0, getmaxx(_window));
    ++row;

    for (auto line = _lines.begin() + _offset, e = _lines.end();
        line != e && row < getmaxy(_window);
        ++line, ++row)
    {
        bool selected = _selectedIndex == row - (headers.size() + 1) + _offset;

        wmove(_window, row, 0);

        attr_t attributes = 0;

        if (selected)
        {
            attributes |= A_REVERSE;
            wchgat(_window, -1, A_REVERSE, 0, NULL);
        }

        if (NCurses::addUtf8String(_window, (*line).c_str(), attributes) > getmaxx(_window))
            NCurses::addCutOffIndicator(_window, attributes);
    }
}

void MessageView::updateStatus()
{
    std::ostringstream status;

    status << "line " << (_selectedIndex + 1) << " of " << _lines.size();

    StatusBar::instance().setStatus(status.str());
}

int MessageView::visibleLines() const
{
    return getmaxy(_window) - headers.size() - 1;
}

int MessageView::lineCount() const
{
    return _lines.size();
}

void MessageView::processMimePart(GMimeObject * part)
{
    GMimeContentType * contentType = g_mime_object_get_content_type(part);

    /* If this part is plain text */
    if (g_mime_content_type_is_type(contentType, "text", "*") &&
        !g_mime_content_type_is_type(contentType, "text", "html"))
    {
        GMimeDataWrapper * content = g_mime_part_get_content_object(GMIME_PART(part));
        const char * charset = g_mime_object_get_content_type_parameter(part, "charset");
        GByteArray * buffer = g_byte_array_sized_new(4096);
        GMimeStream * stream = g_mime_stream_mem_new_with_byte_array(buffer);
        GMimeStream * filter = g_mime_stream_filter_new(stream);

        g_mime_stream_filter_add(GMIME_STREAM_FILTER(filter),
            g_mime_filter_charset_new(charset, "UTF-8"));

        g_mime_data_wrapper_write_to_stream(content, filter);

        char * lineStart = (char *) buffer->data;
        char * lineEnd;

        while ((lineEnd = (char *) memchr(lineStart, '\n',
            buffer->len - (lineStart - ((char *) buffer->data)))) != NULL)
        {
            _lines.push_back(std::string(lineStart, lineEnd - lineStart));

            lineStart = lineEnd + 1;
        }
    }
    else if (g_mime_content_type_is_type(contentType, "multipart", "*"))
    {
        for (int index = 0, count = g_mime_multipart_get_count(GMIME_MULTIPART(part));
            index < count;
            ++index)
        {
            processMimePart(g_mime_multipart_get_part(GMIME_MULTIPART(part), index));
        }
    }
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

