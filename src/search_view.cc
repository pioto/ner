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

#include "search_view.hh"
#include "thread_view.hh"
#include "view_manager.hh"
#include "util.hh"
#include "colors.hh"

const int newestDateWidth = 13;
const int messageCountWidth = 8;
const int authorsWidth = 20;

SearchView::Thread::Thread(notmuch_thread_t * thread)
    : id(notmuch_thread_get_thread_id(thread)),
        subject(notmuch_thread_get_subject(thread)),
        authors(notmuch_thread_get_authors(thread)),
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
    : WindowView(),
        _query(notmuch_query_create(NotMuch::database(), search.c_str())),
        _selectedIndex(0),
        _offset(0)
{
    _doneCollecting = false;
    _thread = std::thread(std::bind(&SearchView::collectThreads, this));

    /* Colors */
    init_pair(Colors::SEARCH_VIEW_DATE,                     COLOR_YELLOW,   COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_MESSAGE_COUNT_COMPLETE,   COLOR_GREEN,    COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_MESSAGE_COUNT_PARTIAL,    COLOR_MAGENTA,  COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_AUTHORS,                  COLOR_CYAN,     COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_SUBJECT,                  COLOR_WHITE,    COLOR_BLACK);
    init_pair(Colors::SEARCH_VIEW_TAGS,                     COLOR_RED,      COLOR_BLACK);

    /* Key Sequences */
    addHandledSequence("j", std::bind(&SearchView::nextThread, this));
    addHandledSequence(KEY_DOWN, std::bind(&SearchView::nextThread, this));
    addHandledSequence("k", std::bind(&SearchView::previousThread, this));
    addHandledSequence(KEY_UP, std::bind(&SearchView::previousThread, this));

    addHandledSequence(KEY_NPAGE, std::bind(&SearchView::nextPage, this));
    addHandledSequence('d' - 96, std::bind(&SearchView::nextPage, this)); // Ctrl-D
    addHandledSequence(KEY_PPAGE, std::bind(&SearchView::previousPage, this));
    addHandledSequence('u' - 96, std::bind(&SearchView::previousPage, this)); // Ctrl-U

    addHandledSequence("gg", std::bind(&SearchView::moveToTop, this));
    addHandledSequence(KEY_HOME, std::bind(&SearchView::moveToTop, this));
    addHandledSequence("G", std::bind(&SearchView::moveToBottom, this));
    addHandledSequence(KEY_END, std::bind(&SearchView::moveToBottom, this));

    addHandledSequence("=", std::bind(&SearchView::refreshThreads, this));

    addHandledSequence("\n", std::bind(&SearchView::openSelectedThread, this));

    std::unique_lock<std::mutex> lock(_mutex);
    while (_threads.size() < getmaxy(_window) && !_doneCollecting)
        _condition.wait_for(lock, std::chrono::milliseconds(50));
}

SearchView::~SearchView()
{
    _thread.join();
    notmuch_query_destroy(_query);
}

void SearchView::update()
{
    if (_offset > _threads.size())
        return;

    int row = 0;

    for (auto thread = _threads.begin() + _offset;
        thread != _threads.end() && row < getmaxy(_window);
        ++thread, ++row)
    {
        bool selected = row + _offset == _selectedIndex;
        bool unread = (*thread).tags.find("unread") != (*thread).tags.end();
        bool completeMatch = (*thread).matchedMessages == (*thread).totalMessages;

        if (selected)
            wattron(_window, A_REVERSE);

        if (unread)
            wattron(_window, A_BOLD);

        wmove(_window, row, 0);

        /* Date */
        wattron(_window, COLOR_PAIR(Colors::SEARCH_VIEW_DATE));
        waddstr(_window, relativeTime((*thread).newestDate).c_str());
        wattroff(_window, COLOR_PAIR(Colors::SEARCH_VIEW_DATE));

        while (getcurx(_window) < newestDateWidth)
            waddch(_window, ' ');

        /* Message Count */
        waddch(_window, '[');
        if (completeMatch)
            wattron(_window, COLOR_PAIR(Colors::SEARCH_VIEW_MESSAGE_COUNT_COMPLETE));
        else
            wattron(_window, COLOR_PAIR(Colors::SEARCH_VIEW_MESSAGE_COUNT_PARTIAL));
        wprintw(_window, "%u/%u",
            (*thread).matchedMessages,
            (*thread).totalMessages);
        if (completeMatch)
            wattroff(_window, COLOR_PAIR(Colors::SEARCH_VIEW_MESSAGE_COUNT_COMPLETE));
        else
            wattroff(_window, COLOR_PAIR(Colors::SEARCH_VIEW_MESSAGE_COUNT_PARTIAL));
        waddch(_window, ']');

        while (getcurx(_window) < newestDateWidth + messageCountWidth)
            waddch(_window, ' ');

        /* Authors */
        wattron(_window, COLOR_PAIR(Colors::SEARCH_VIEW_AUTHORS));
        waddnstr(_window, (*thread).authors.c_str(), authorsWidth - 1);
        wattroff(_window, COLOR_PAIR(Colors::SEARCH_VIEW_AUTHORS));

        while (getcurx(_window) < newestDateWidth + messageCountWidth + authorsWidth)
            waddch(_window, ' ');

        /* Subject */
        wattron(_window, COLOR_PAIR(Colors::SEARCH_VIEW_SUBJECT));
        waddnstr(_window, (*thread).subject.c_str(), getmaxx(_window) - getcurx(_window));
        wattroff(_window, COLOR_PAIR(Colors::SEARCH_VIEW_SUBJECT));

        waddch(_window, ' ');

        /* Tags */
        std::ostringstream tagStream;
        std::copy((*thread).tags.begin(), (*thread).tags.end(),
            std::ostream_iterator<std::string>(tagStream, " "));
        std::string tags(tagStream.str());

        if (tags.size() > 0)
            /* Get rid of the trailing space */
            tags.resize(tags.size() - 1);

        wattron(_window, COLOR_PAIR(Colors::SEARCH_VIEW_TAGS));
        waddnstr(_window, tags.c_str(), getmaxx(_window) - getcurx(_window));
        wattroff(_window, COLOR_PAIR(Colors::SEARCH_VIEW_TAGS));

        if (selected)
        {
            whline(_window, ' ', getmaxx(_window));
            wattroff(_window, A_REVERSE);
        }
        else
            wclrtoeol(_window);

        if (unread)
            wattroff(_window, A_BOLD);
    }
}

void SearchView::resize()
{
    WindowView::resize();

    makeSelectionVisible();
}

void SearchView::nextThread()
{
    if (_selectedIndex < _threads.size() - 1)
        ++_selectedIndex;

    makeSelectionVisible();
}

void SearchView::previousThread()
{
    if (_selectedIndex > 0)
        --_selectedIndex;

    makeSelectionVisible();
}

void SearchView::nextPage()
{
    if (_selectedIndex + getmaxy(_window) >= _threads.size())
        _selectedIndex = _threads.size() - 1;
    else
        _selectedIndex += getmaxy(_window) - 1;

    makeSelectionVisible();
}

void SearchView::previousPage()
{
    if (getmaxy(_window) > _selectedIndex)
        _selectedIndex = 0;
    else
        _selectedIndex -= getmaxy(_window) - 1;

    makeSelectionVisible();
}

void SearchView::moveToTop()
{
    _selectedIndex = 0;

    makeSelectionVisible();
}

void SearchView::moveToBottom()
{
    _selectedIndex = _threads.size() - 1;

    makeSelectionVisible();
}

void SearchView::openSelectedThread()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _viewManager->addView(new ThreadView((*(_threads.begin() + _selectedIndex)).id));
}

void SearchView::refreshThreads()
{
    if (_threads.empty())
    {
        collectThreads();
        return;
    }

    std::string selectedId = (*(_threads.begin() + _selectedIndex)).id;

    _threads.clear();
    collectThreads();

    bool found = false;

    for (auto i = _threads.begin(), e = _threads.end(); i != e; ++i)
    {
        if ((*i).id == selectedId)
        {
            _selectedIndex = i - _threads.begin();
            found = true;
            break;
        }
    }

    if (!found)
    {
        if (_threads.size() <= _selectedIndex)
            _selectedIndex = _threads.size() - 1;
    }

    makeSelectionVisible();
}

void SearchView::makeSelectionVisible()
{
    if (_selectedIndex < _offset)
        _offset = _selectedIndex;
    else if (_selectedIndex >= _offset + getmaxy(_window))
        _offset = _selectedIndex - getmaxy(_window) + 1;
}

void SearchView::collectThreads()
{
    std::unique_lock<std::mutex> lock(_mutex);
    lock.unlock();

    notmuch_threads_t * _threadIterator;

    int count = 0;

    for (_threadIterator = notmuch_query_search_threads(_query);
        notmuch_threads_valid(_threadIterator);
        notmuch_threads_move_to_next(_threadIterator), ++count)
    {
        lock.lock();
        _threads.push_back(notmuch_threads_get(_threadIterator));

        if (count % 50 == 0)
            _condition.notify_one();

        lock.unlock();
    }

    _doneCollecting = true;
    notmuch_threads_destroy(_threadIterator);

    /* For cases when there are no matching threads */
    _condition.notify_one();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

