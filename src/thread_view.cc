/* ner: src/thread_view.cc
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

#include "thread_view.hh"
#include "notmuch.hh"
#include "util.hh"

static const uint32_t threadViewHeight = 12;

ThreadView::Message::Message()
    : date(0), matched(false), parent(0)
{
}

ThreadView::Message::Message(notmuch_message_t * message, Message * parentMessage)
    : parent(parentMessage),
        id(notmuch_message_get_message_id(message)),
        filename(notmuch_message_get_filename(message)),
        date(notmuch_message_get_date(message)),
        matched(notmuch_message_get_flag(message, NOTMUCH_MESSAGE_FLAG_MATCH))
{
    headers["From"]     = notmuch_message_get_header(message, "From");
    headers["To"]       = notmuch_message_get_header(message, "To");
    headers["Subject"]  = notmuch_message_get_header(message, "Subject");

    for (notmuch_messages_t * messages = notmuch_message_get_replies(message);
        notmuch_messages_valid(messages);
        notmuch_messages_move_to_next(messages))
    {
        replies.push_back(Message(notmuch_messages_get(messages), this));
    }
}

ThreadView::ThreadView(const std::string & id)
    : View(),
        _selectedMessage(&_topMessage),
        _threadWindow(newwin(threadViewHeight, COLS, 0, 0)),
        _messageWindow(newwin(LINES - threadViewHeight - 1, COLS, threadViewHeight, 0))
{
        _query = notmuch_query_create(NotMuch::database(), std::string("thread:").append(id).c_str());
        _thread = notmuch_threads_get(notmuch_query_search_threads(_query));
        _topMessage = notmuch_messages_get(notmuch_thread_get_toplevel_messages(_thread));
}

ThreadView::~ThreadView()
{
    delwin(_threadWindow);
    delwin(_messageWindow);
}

void ThreadView::update()
{
    std::vector<chtype> leading;

    displayMessageLine(_topMessage, 0, leading, true, 0);

    wattron(_threadWindow, COLOR_PAIR(NER_COLOR_BLUE));
    mvwhline(_threadWindow, threadViewHeight - 1, 0, 0, COLS);
    wattroff(_threadWindow, COLOR_PAIR(NER_COLOR_BLUE));

    std::ifstream messageFile;
    messageFile.open(_selectedMessage->filename, std::ifstream::in);

    wmove(_messageWindow, 0, 0);
    for (uint32_t line = 0; line < getmaxy(_messageWindow) && messageFile.good(); ++line)
    {
        std::string line;
        std::getline(messageFile, line);
        waddstr(_messageWindow, line.c_str());
        wmove(_messageWindow, getcury(_messageWindow) + 1, 0);
    }

    messageFile.close();
}

void ThreadView::refresh()
{
    wrefresh(_threadWindow);
    wrefresh(_messageWindow);
}

void ThreadView::focus()
{
    wclear(_threadWindow);
    clearok(_threadWindow, true);
}

void ThreadView::handleKeyPress(const int key)
{
}

uint32_t ThreadView::displayMessageLine(const Message & message,
    uint32_t start, std::vector<chtype> & leading, bool last, uint32_t row)
{
    if (start <= row)
    {
        wmove(_threadWindow, row - start, 0);

        if (&message == _selectedMessage)
            wattron(_threadWindow, A_REVERSE);

        wattron(_threadWindow, COLOR_PAIR(NER_COLOR_GREEN));

        for (auto character = leading.begin();
            character != leading.end();
            ++character)
        {
            waddch(_threadWindow, *character);
        }

        if (last)
            waddch(_threadWindow, ACS_LLCORNER);
        else
            waddch(_threadWindow, ACS_LTEE);

        waddch(_threadWindow, '>');
        waddch(_threadWindow, ' ');

        wattroff(_threadWindow, COLOR_PAIR(NER_COLOR_GREEN));

        std::string messageLabel((*message.headers.find("From")).second);
        messageLabel.resize(getmaxx(_threadWindow) - getcurx(_threadWindow), ' ');
        waddstr(_threadWindow, messageLabel.c_str());

        if (&message == _selectedMessage)
        {
            wattroff(_threadWindow, A_REVERSE);
        }

    }

    ++row;

    if (last)
        leading.push_back(' ');
    else
        leading.push_back(ACS_VLINE);

    for (auto iterator = message.replies.begin();
        iterator != message.replies.end() && row < getmaxy(_threadWindow) - 1;
        ++iterator)
    {
        row = displayMessageLine(*iterator, start, leading, (iterator + 1) == message.replies.end(), row);
    }

    leading.pop_back();

    return row;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

