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

#include <sstream>

#include "thread_view.hh"
#include "notmuch.hh"
#include "util.hh"
#include "colors.hh"
#include "view_manager.hh"
#include "message_view.hh"
#include "status_bar.hh"

ThreadView::Message::Message(notmuch_message_t * message)
    : id(notmuch_message_get_message_id(message)),
        filename(notmuch_message_get_filename(message)),
        date(notmuch_message_get_date(message)),
        matched(notmuch_message_get_flag(message, NOTMUCH_MESSAGE_FLAG_MATCH)),
        headers{
            {"From",    notmuch_message_get_header(message, "From")     ? : "(null)"},
            {"To",      notmuch_message_get_header(message, "To")       ? : "(null)"},
            {"Subject", notmuch_message_get_header(message, "Subject")  ? : "(null)"},
        }
{
    for (notmuch_messages_t * messages = notmuch_message_get_replies(message);
        notmuch_messages_valid(messages);
        notmuch_messages_move_to_next(messages))
    {
        replies.push_back(Message(notmuch_messages_get(messages)));
    }
}

ThreadView::ThreadView(notmuch_thread_t * thread)
    : LineBrowserView()
{
    _topMessage = notmuch_messages_get(notmuch_thread_get_toplevel_messages(thread));
    _messageCount = notmuch_thread_get_total_messages(thread);

    /* Key Sequences */
    addHandledSequence("\n", std::bind(&ThreadView::openSelectedMessage, this));

    /* Colors */
    init_pair(Colors::THREAD_VIEW_ARROW,        COLOR_GREEN,    COLOR_BLACK);
}

ThreadView::~ThreadView()
{
}

ThreadView * ThreadView::fromId(const std::string & threadId)
{
    ThreadView * threadView;

    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_query_t * query = notmuch_query_create(database, ("thread:" + threadId).c_str());
    notmuch_threads_t * threads = notmuch_query_search_threads(query);

    if (notmuch_threads_valid(threads))
        threadView = new ThreadView(notmuch_threads_get(threads));
    else
    {
        threadView = 0;
        StatusBar::instance().displayMessage("Cannot find thread with ID: " + threadId);
    }

    notmuch_threads_destroy(threads);
    notmuch_query_destroy(query);

    notmuch_database_close(database);

    return threadView;
}

void ThreadView::update()
{
    std::vector<chtype> leading;

    werase(_window);

    displayMessageLine(_topMessage, leading, true, 0);
}

void ThreadView::openSelectedMessage()
{
    MessageView * messageView = MessageView::fromId(selectedMessage().id);

    if (messageView)
        _viewManager->addView(messageView);
}

void ThreadView::updateStatus()
{
    std::ostringstream status;

    status << "message " << (_selectedIndex + 1) << " of " << _messageCount;

    StatusBar::instance().setStatus(status.str());
}

uint32_t ThreadView::displayMessageLine(const Message & message,
    std::vector<chtype> & leading, bool last, int index)
{
    if (index >= _offset)
    {
        bool selected = index == _selectedIndex;

        wmove(_window, index - _offset, 0);

        if (selected)
            wattron(_window, A_REVERSE);

        wattron(_window, COLOR_PAIR(Colors::THREAD_VIEW_ARROW));

        for (auto character = leading.begin();
            character != leading.end();
            ++character)
        {
            waddch(_window, *character);
        }

        if (last)
            waddch(_window, ACS_LLCORNER);
        else
            waddch(_window, ACS_LTEE);

        waddch(_window, '>');
        waddch(_window, ' ');

        wattroff(_window, COLOR_PAIR(Colors::THREAD_VIEW_ARROW));

        std::string messageLabel((*message.headers.find("From")).second);
        messageLabel.resize(getmaxx(_window) - getcurx(_window), ' ');
        waddstr(_window, messageLabel.c_str());

        if (selected)
        {
            wattroff(_window, A_REVERSE);
        }
    }

    ++index;

    if (last)
        leading.push_back(' ');
    else
        leading.push_back(ACS_VLINE);

    for (auto reply = message.replies.begin(), e = message.replies.end();
        reply != e && index < getmaxy(_window) + _offset;
        ++reply)
    {
        index = displayMessageLine(*reply, leading, (reply + 1) == e, index);
    }

    leading.pop_back();

    return index;
}

int ThreadView::lineCount() const
{
    return _messageCount;
}

const ThreadView::Message & ThreadView::selectedMessage() const
{
    std::vector<const Message *> messages{ &_topMessage };

    const Message * message = messages.back();

    for (int index = 0; index < _selectedIndex; ++index)
    {
        messages.pop_back();

        if (!message->replies.empty())
        {
            for (auto reply = message->replies.rbegin(), e = message->replies.rend();
                reply != e;
                ++reply)
            {
                messages.push_back(&(*reply));
            }
        }

        message = messages.back();
    }

    return *message;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

