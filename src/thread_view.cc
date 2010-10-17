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
#include "reply_view.hh"

ThreadView::ThreadView(const std::string & threadId, const View::Geometry & geometry)
    : LineBrowserView(geometry), _id(threadId)
{
    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_query_t * query = notmuch_query_create(database, ("thread:" + threadId).c_str());
    notmuch_threads_t * threads = notmuch_query_search_threads(query);
    notmuch_thread_t * thread = 0;
    notmuch_messages_t * messages;

    if (!notmuch_threads_valid(threads) || !(thread = notmuch_threads_get(threads)))
    {
        notmuch_threads_destroy(threads);
        notmuch_query_destroy(query);

        notmuch_database_close(database);

        throw NotMuch::InvalidThreadException(threadId);
    }

    for (messages = notmuch_thread_get_toplevel_messages(thread);
        notmuch_messages_valid(messages);
        notmuch_messages_move_to_next(messages))
    {
        _topMessages.push_back(notmuch_messages_get(messages));
    }

    notmuch_messages_destroy(messages);
    notmuch_threads_destroy(threads);
    notmuch_query_destroy(query);

    notmuch_database_close(database);

    _messageCount = notmuch_thread_get_total_messages(thread);

    _selectedIndex = 0;

    /* Find first unread message */
    int messageIndex = 0;

    for (NotMuch::Message::const_iterator message(_topMessages.rbegin(), _topMessages.rend()), e;
        message != e; ++message, ++messageIndex)
    {
        if (message->tags.find("unread") != message->tags.end())
        {
            _selectedIndex = messageIndex;
            break;
        }
    }

    makeSelectionVisible();

    /* Key Sequences */
    addHandledSequence("\n", std::bind(&ThreadView::openSelectedMessage, this));
    addHandledSequence("r", std::bind(&ThreadView::reply, this));
}

ThreadView::~ThreadView()
{
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
        "thread:" + _id,
        messagePosition.str()
    };
}

void ThreadView::openSelectedMessage()
{
    try
    {
        std::shared_ptr<MessageView> messageView(new MessageView());
        messageView->setMessage(selectedMessage().id);
        ViewManager::instance().addView(messageView);
    }
    catch (const NotMuch::InvalidMessageException & e)
    {
        StatusBar::instance().displayMessage(e.what());
    }
}

const NotMuch::Message & ThreadView::selectedMessage() const
{
    NotMuch::Message::const_iterator message(_topMessages.rbegin(), _topMessages.rend());
    std::advance(message, _selectedIndex);
    return *message;
}

void ThreadView::reply()
{
    try
    {
        ViewManager::instance().addView(std::shared_ptr<ReplyView>(new ReplyView(selectedMessage().id)));
    }
    catch (const NotMuch::InvalidMessageException & e)
    {
        StatusBar::instance().displayMessage(e.what());
    }
}

int ThreadView::lineCount() const
{
    return _messageCount;
}

uint32_t ThreadView::displayMessageLine(const NotMuch::Message & message,
    std::vector<chtype> & leading, bool last, int index)
{
    if (index >= _offset)
    {
        try
        {
            bool selected = index == _selectedIndex;
            bool unread = message.tags.find("unread") != message.tags.end();

            int x = 0;
            int row = index - _offset;

            wmove(_window, row, x);

            attr_t attributes = 0;

            if (selected)
                attributes |= A_REVERSE;

            if (unread)
                attributes |= A_BOLD;

            wchgat(_window, -1, attributes, 0, NULL);

            x += NCurses::addPlainString(_window, leading.begin(), leading.end(),
                attributes, ColorID::ThreadViewArrow);

            NCurses::checkMove(_window, x);

            x += NCurses::addChar(_window, last ? ACS_LLCORNER : ACS_LTEE,
                attributes, ColorID::ThreadViewArrow);

            NCurses::checkMove(_window, x);

            x += NCurses::addChar(_window, '>', attributes, ColorID::ThreadViewArrow);

            NCurses::checkMove(_window, ++x);

            /* Sender */
            x += NCurses::addUtf8String(_window, (*message.headers.find("From")).second.c_str(),
                attributes);

            NCurses::checkMove(_window, ++x);

            /* Date */
            x += NCurses::addPlainString(_window, relativeTime(message.date),
                attributes, ColorID::ThreadViewDate);

            NCurses::checkMove(_window, ++x);

            /* Tags */
            std::ostringstream tagStream;
            std::copy(message.tags.begin(), message.tags.end(),
                std::ostream_iterator<std::string>(tagStream, " "));
            std::string tags(tagStream.str());

            if (tags.size() > 0)
                /* Get rid of the trailing space */
                tags.resize(tags.size() - 1);

            x += NCurses::addPlainString(_window, tags, attributes, ColorID::ThreadViewTags);

            NCurses::checkMove(_window, x - 1);
        }
        catch (const NCurses::CutOffException & e)
        {
            NCurses::addCutOffIndicator(_window);
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

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

