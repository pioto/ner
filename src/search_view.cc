/* ner: src/search_view.cc
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
#include <algorithm>
#include <chrono>
#include <iterator>
#include <sched.h>

#include "search_view.hh"
#include "thread_view.hh"
#include "view_manager.hh"
#include "util.hh"
#include "colors.hh"
#include "ncurses.hh"
#include "notmuch.hh"
#include "status_bar.hh"

const int newestDateWidth = 13;
const int messageCountWidth = 8;
const int authorsWidth = 20;

const auto conditionWaitTime = std::chrono::milliseconds(50);

SearchView::Thread::Thread(notmuch_thread_t * thread)
    : id(notmuch_thread_get_thread_id(thread)),
        subject(notmuch_thread_get_subject(thread) ? : "(null)"),
        authors(notmuch_thread_get_authors(thread) ? : "(null)"),
        totalMessages(notmuch_thread_get_total_messages(thread)),
        matchedMessages(notmuch_thread_get_matched_messages(thread)),
        newestDate(notmuch_thread_get_newest_date(thread)),
        oldestDate(notmuch_thread_get_oldest_date(thread))
{
    notmuch_tags_t * tagIterator;

    for (tagIterator = notmuch_thread_get_tags(thread);
        notmuch_tags_valid(tagIterator);
        notmuch_tags_move_to_next(tagIterator))
    {
        tags.insert(notmuch_tags_get(tagIterator));
    }

    notmuch_tags_destroy(tagIterator);
}

SearchView::SearchView(const std::string & search)
    : LineBrowserView(),
        _searchTerms(search)
{
    _collecting = true;
    _thread = std::thread(std::bind(&SearchView::collectThreads, this));

    /* Colors */
    init_pair(Colors::SEARCH_VIEW_DATE,                     COLOR_YELLOW,   COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_MESSAGE_COUNT_COMPLETE,   COLOR_GREEN,    COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_MESSAGE_COUNT_PARTIAL,    COLOR_MAGENTA,  COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_AUTHORS,                  COLOR_CYAN,     COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_SUBJECT,                  COLOR_WHITE,    COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_TAGS,                     COLOR_RED,      COLOR_BLACK);

    /* Key Sequences */
    addHandledSequence("=", std::bind(&SearchView::refreshThreads, this));
    addHandledSequence("\n", std::bind(&SearchView::openSelectedThread, this));

    std::unique_lock<std::mutex> lock(_mutex);
    while (_threads.size() < getmaxy(_window) && _collecting)
        _condition.wait_for(lock, conditionWaitTime);
}

SearchView::~SearchView()
{
    if (_thread.joinable())
    {
        _collecting = false;
        _thread.join();
    }
}

void SearchView::update()
{
    werase(_window);

    if (_offset > _threads.size())
        return;

    int row = 0;

    for (auto thread = _threads.begin() + _offset;
        thread != _threads.end() && row < getmaxy(_window);
        ++thread, ++row)
    {
#define CHECK_CONTINUE(amount) \
    if ((amount) >= getmaxx(_window)) \
    { \
        NCurses::addCutOffIndicator(_window, attributes); \
        continue; \
    }

        bool selected = row + _offset == _selectedIndex;
        bool unread = (*thread).tags.find("unread") != (*thread).tags.end();
        bool completeMatch = (*thread).matchedMessages == (*thread).totalMessages;

        int x = 0;

        wmove(_window, row, x);

        attr_t attributes = 0;

        if (unread)
            attributes |= A_BOLD;

        if (selected)
        {
            attributes |= A_REVERSE;
            wchgat(_window, -1, A_REVERSE, 0, NULL);
        }

        /* Date */
        NCurses::addPlainString(_window, relativeTime((*thread).newestDate),
            attributes, Colors::SEARCH_VIEW_DATE, newestDateWidth - 1);

        CHECK_CONTINUE(x += newestDateWidth)
        wmove(_window, row, x);

        /* Message Count */
        std::ostringstream messageCountStream;
        messageCountStream << (*thread).matchedMessages << '/' << (*thread).totalMessages;

        x += NCurses::addChar(_window, '[', attributes);

        CHECK_CONTINUE(x)

        x += NCurses::addPlainString(_window, messageCountStream.str(),
            attributes, completeMatch ? Colors::SEARCH_VIEW_MESSAGE_COUNT_COMPLETE :
                                        Colors::SEARCH_VIEW_MESSAGE_COUNT_PARTIAL,
            messageCountWidth - 1);

        CHECK_CONTINUE(x)
        wmove(_window, row, x);

        NCurses::addChar(_window, ']', attributes);

        CHECK_CONTINUE(x = newestDateWidth + messageCountWidth)
        wmove(_window, row, x);

        /* Authors */
        NCurses::addUtf8String(_window, (*thread).authors.c_str(),
            attributes, Colors::SEARCH_VIEW_AUTHORS, authorsWidth - 1);

        CHECK_CONTINUE(x += authorsWidth)
        wmove(_window, row, x);

        /* Subject */
        x += NCurses::addUtf8String(_window, (*thread).subject.c_str(),
            attributes, Colors::SEARCH_VIEW_SUBJECT);

        CHECK_CONTINUE(++x)
        wmove(_window, row, x);

        /* Tags */
        std::ostringstream tagStream;
        std::copy((*thread).tags.begin(), (*thread).tags.end(),
            std::ostream_iterator<std::string>(tagStream, " "));
        std::string tags(tagStream.str());

        if (tags.size() > 0)
            /* Get rid of the trailing space */
            tags.resize(tags.size() - 1);

        x += NCurses::addPlainString(_window, tags, attributes, Colors::SEARCH_VIEW_TAGS);

        if (x > getmaxx(_window))
            NCurses::addCutOffIndicator(_window, attributes);

#undef CHECK_CONTINUE
    }
}

std::vector<std::string> SearchView::status() const
{
    std::ostringstream threadPosition;

    if (_threads.size() > 0)
        threadPosition << "thread " << (_selectedIndex + 1) << " of " << _threads.size();
    else
        threadPosition << "no matching threads";

    return std::vector<std::string>{
        "search-terms: \"" + _searchTerms + '"',
        threadPosition.str()
    };
}

void SearchView::openSelectedThread()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_selectedIndex < _threads.size())
    {
        ThreadView * threadView = ThreadView::fromId(_threads.at(_selectedIndex).id);

        if (threadView)
            ViewManager::instance().addView(threadView);
    }
}

void SearchView::refreshThreads()
{
    /* If the thread is still going, stop it, and wait for it to return */
    if (_thread.joinable())
    {
        _collecting = false;
        _thread.join();
    }

    bool empty = _threads.empty();
    std::string selectedId;

    if (!empty)
        selectedId = (*(_threads.begin() + _selectedIndex)).id;

    _threads.clear();

    /* Start collecting threads in the background */
    _collecting = true;
    _thread = std::thread(std::bind(&SearchView::collectThreads, this));

    /* Locate the previously selected thread ID */
    bool found = false;
    std::unique_lock<std::mutex> lock(_mutex);

    if (empty)
        found = true;
    else
    {
        int index = 0;

        while (!found && _collecting)
        {
            for (; index < _threads.size(); ++index)
            {
                /* Stop if we found the thread ID */
                if (_threads.at(index).id == selectedId)
                {
                    found = true;
                    _selectedIndex = index;
                    break;
                }
            }

            _condition.wait_for(lock, conditionWaitTime);
        }
    }

    /* Wait until we have enough threads to fill the screen */
    while (_threads.size() - _offset < getmaxy(_window) && _collecting)
        _condition.wait_for(lock, conditionWaitTime);

    /* If we didn't find it, make sure the selected index is valid */
    if (!found)
    {
        if (_threads.size() <= _selectedIndex)
            _selectedIndex = _threads.size() - 1;
    }

    updateStatus();
    makeSelectionVisible();
}

int SearchView::lineCount() const
{
    return _threads.size();
}

void SearchView::collectThreads()
{
    std::unique_lock<std::mutex> lock(_mutex);
    lock.unlock();

    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_query_t * query = notmuch_query_create(database, _searchTerms.c_str());
    notmuch_threads_t * threadIterator;

    for (threadIterator = notmuch_query_search_threads(query);
        notmuch_threads_valid(threadIterator) && _collecting;
        notmuch_threads_move_to_next(threadIterator))
    {
        lock.lock();

        notmuch_thread_t * thread = notmuch_threads_get(threadIterator);
        _threads.push_back(thread);
        notmuch_thread_destroy(thread);

        _condition.notify_one();

        lock.unlock();

        sched_yield();
    }

    _collecting = false;
    notmuch_threads_destroy(threadIterator);
    notmuch_query_destroy(query);
    notmuch_database_close(database);

    /* For cases when there are no matching threads */
    _condition.notify_one();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

