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

#include "search_view.hh"
#include "thread_view.hh"
#include "view_manager.hh"
#include "util.hh"

/* Things that should be configurable */
const uint16_t newestDateWidth = 13;
const uint16_t messageCountWidth = 8;

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
        std::string subject = (*thread).subject;
        std::string newestDate = relativeTime((*thread).newestDate);

        subject.resize(getmaxx(_window) - (newestDateWidth + messageCountWidth), ' ');
        newestDate.resize(newestDateWidth, ' ');

        if (row + _offset == _selectedIndex)
            wattron(_window, A_REVERSE);
        wattron(_window, A_BOLD | COLOR_PAIR(NER_COLOR_YELLOW));
        mvwaddstr(_window, row, 0, newestDate.c_str());
        wattroff(_window, A_BOLD | COLOR_PAIR(NER_COLOR_YELLOW));

        mvwaddch(_window, row, newestDateWidth, '[');
        wattron(_window, COLOR_PAIR(NER_COLOR_CYAN));
        wprintw(_window, "%u/%u",
            (*thread).matchedMessages,
            (*thread).totalMessages);
        wattroff(_window, COLOR_PAIR(NER_COLOR_CYAN));
        waddch(_window, ']');
        while (getcurx(_window) < newestDateWidth + messageCountWidth)
            waddch(_window, ' ');

        mvwaddstr(_window, row, newestDateWidth + messageCountWidth, subject.c_str());
        if (row + _offset == _selectedIndex)
            wattroff(_window, A_REVERSE);
    }
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
    _viewManager->addView(new ThreadView((*(_threads.begin() + _selectedIndex)).id));
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

    for (_threadIterator = notmuch_query_search_threads(_query);
        notmuch_threads_valid(_threadIterator);
        notmuch_threads_move_to_next(_threadIterator))
    {
        lock.lock();
        _threads.push_back(notmuch_threads_get(_threadIterator));
        _condition.notify_one();
        lock.unlock();
    }

    _doneCollecting = true;
    notmuch_threads_destroy(_threadIterator);

    /* For cases when there are no matching threads */
    _condition.notify_one();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

