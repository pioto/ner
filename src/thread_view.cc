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
#include <iterator>

#include "thread_view.hh"
#include "notmuch.hh"
#include "util.hh"
#include "colors.hh"
#include "ncurses.hh"
#include "view_manager.hh"
#include "message_view.hh"
#include "status_bar.hh"

ThreadView::ThreadView(notmuch_thread_t * thread, int x, int y, int width, int height)
    : LineBrowserView(x, y, width, height),
        _id(notmuch_thread_get_thread_id(thread))
{
    notmuch_messages_t * messages;

    for (messages = notmuch_thread_get_toplevel_messages(thread);
        notmuch_messages_valid(messages);
        notmuch_messages_move_to_next(messages))
    {
        _topMessages.push_back(notmuch_messages_get(messages));
    }

    notmuch_messages_destroy(messages);

    _messageCount = notmuch_thread_get_total_messages(thread);

    /* Key Sequences */
    addHandledSequence("\n", std::bind(&ThreadView::openSelectedMessage, this));

    /* Colors */
    init_pair(Colors::THREAD_VIEW_ARROW,        COLOR_GREEN,    COLOR_BLACK);
    init_pair(Colors::THREAD_VIEW_DATE,         COLOR_CYAN,     COLOR_BLACK);
    init_pair(Colors::THREAD_VIEW_TAGS,         COLOR_RED,      COLOR_BLACK);
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

    int index = 0;

    for (auto message = _topMessages.begin(), e = _topMessages.end();
        message != e && index < getmaxy(_window) + _offset;
        ++message)
    {
        index = displayMessageLine(*message, leading, (message + 1) == e, index);
    }
}

std::vector<std::string> ThreadView::status() const
{
    std::ostringstream messagePosition;

    messagePosition << "message " << (_selectedIndex + 1) << " of " << _messageCount;

    return std::vector<std::string>{
        "thread-id: " + _id,
        messagePosition.str()
    };
}

void ThreadView::openSelectedMessage()
{
    MessageView * messageView = MessageView::fromId(selectedMessage().id);

    if (messageView)
        ViewManager::instance().addView(messageView);
}

uint32_t ThreadView::displayMessageLine(const NotMuch::Message & message,
    std::vector<chtype> & leading, bool last, int index)
{
    if (index >= _offset)
    {
        for (;;) // So that we can break when needed
        {
#define CHECK_BREAK(amount) \
    if ((amount) >= getmaxx(_window)) \
    { \
        NCurses::addCutOffIndicator(_window, attributes); \
        break; \
    }

            bool selected = index == _selectedIndex;
            bool unread = message.tags.find("unread") != message.tags.end();

            int x = 0;
            int row = index - _offset;

            wmove(_window, row, x);

            attr_t attributes = 0;

            if (selected)
            {
                attributes |= A_REVERSE;
                wchgat(_window, -1, A_REVERSE, 0, NULL);
            }

            if (unread)
                attributes |= A_BOLD;

            x += NCurses::addPlainString(_window, leading.begin(), leading.end(),
                attributes, Colors::THREAD_VIEW_ARROW);

            CHECK_BREAK(x)
            wmove(_window, row, x);

            x += NCurses::addChar(_window, last ? ACS_LLCORNER : ACS_LTEE,
                attributes, Colors::THREAD_VIEW_ARROW);

            CHECK_BREAK(x)
            wmove(_window, row, x);

            x += NCurses::addChar(_window, '>', attributes, Colors::THREAD_VIEW_ARROW);

            CHECK_BREAK(++x)
            wmove(_window, row, x);

            /* Sender */
            x += NCurses::addUtf8String(_window, (*message.headers.find("From")).second.c_str(),
                attributes);

            CHECK_BREAK(++x)
            wmove(_window, row, x);

            /* Date */
            x += NCurses::addPlainString(_window, relativeTime(message.date),
                attributes, Colors::THREAD_VIEW_DATE);

            CHECK_BREAK(++x)
            wmove(_window, row, x);

            /* Tags */
            std::ostringstream tagStream;
            std::copy(message.tags.begin(), message.tags.end(),
                std::ostream_iterator<std::string>(tagStream, " "));
            std::string tags(tagStream.str());

            if (tags.size() > 0)
                /* Get rid of the trailing space */
                tags.resize(tags.size() - 1);

            x += NCurses::addPlainString(_window, tags, attributes, Colors::THREAD_VIEW_TAGS);

            if (x > getmaxx(_window))
                NCurses::addCutOffIndicator(_window, attributes);

            break;

#undef CHECK_BREAK
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

const NotMuch::Message & ThreadView::selectedMessage() const
{
    std::vector<const NotMuch::Message *> messages;

    std::transform(_topMessages.rbegin(), _topMessages.rend(),
        std::back_inserter(messages), addressOf<NotMuch::Message>());

    const NotMuch::Message * message = messages.back();

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

