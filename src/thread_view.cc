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
#include "colors.hh"
#include "status_bar.hh"

const uint32_t messageListHeight = 8;

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
        _threadWindow(newwin(messageListHeight, COLS, 0, 0)),
        _messageWindow(newwin(LINES - messageListHeight - StatusBar::instance().height(),
            COLS, messageListHeight, 0))
{
    _query = notmuch_query_create(NotMuch::database(), std::string("thread:").append(id).c_str());
    _thread = notmuch_threads_get(notmuch_query_search_threads(_query));
    _topMessage = notmuch_messages_get(notmuch_thread_get_toplevel_messages(_thread));

    /* Colors */
    init_pair(Colors::THREAD_VIEW_ARROW,        COLOR_GREEN,    COLOR_BLACK);
    init_pair(Colors::THREAD_VIEW_SEPARATOR,    COLOR_CYAN,     COLOR_BLACK);
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

    wattron(_threadWindow, COLOR_PAIR(Colors::THREAD_VIEW_SEPARATOR));
    mvwhline(_threadWindow, messageListHeight - 1, 0, 0, COLS);
    wattroff(_threadWindow, COLOR_PAIR(Colors::THREAD_VIEW_SEPARATOR));

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

void ThreadView::resize()
{
    wresize(_messageWindow, messageListHeight, COLS);
    wresize(_threadWindow, LINES - messageListHeight - StatusBar::instance().height(), COLS);

    mvwin(_threadWindow, messageListHeight, 0);
}

uint32_t ThreadView::displayMessageLine(const Message & message,
    uint32_t start, std::vector<chtype> & leading, bool last, uint32_t row)
{
    if (start <= row)
    {
        wmove(_threadWindow, row - start, 0);

        if (&message == _selectedMessage)
            wattron(_threadWindow, A_REVERSE);

        wattron(_threadWindow, COLOR_PAIR(Colors::THREAD_VIEW_ARROW));

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

        wattroff(_threadWindow, COLOR_PAIR(Colors::THREAD_VIEW_ARROW));

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

    for (auto reply = message.replies.begin(), e = message.replies.end();
        reply != e && row < getmaxy(_threadWindow) - 1;
        ++reply)
    {
        row = displayMessageLine(*reply, start, leading, (reply + 1) == e, row);
    }

    leading.pop_back();

    return row;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

