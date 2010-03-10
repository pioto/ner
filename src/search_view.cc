/* ner: src/search_view.cc
 *
 * Copyright (c) 2010 Michael Forney
 *
 * This file is a part of ner.
 *
 * ner is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2, as published by the Free
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

#include "search_view.hh"
#include "thread_view.hh"
#include "view_manager.hh"
#include "util.hh"

SearchView::SearchView(const std::string & search)
    : WindowView(),
        _query(notmuch_query_create(NotMuch::database(), search.c_str())),
        _selectedRow(0),
        _offset(0)
{
}

SearchView::~SearchView()
{
    notmuch_query_destroy(_query);
}

void SearchView::update()
{
    notmuch_threads_t * threads = notmuch_query_search_threads(_query);
    notmuch_thread_t * thread;
    std::string dateTime;
    std::string messageCounts;
    std::string subject;

    for (uint32_t offset = 0; offset < _offset; ++offset)
    {
        if (notmuch_threads_valid(threads))
        {
            notmuch_threads_move_to_next(threads);
        }
        else
        {
            _offset = offset;
            break;
        }
    }

    for (uint32_t row = 0;
        notmuch_threads_valid(threads), row < getmaxy(_window);
        notmuch_threads_move_to_next(threads), ++row)
    {
        thread = notmuch_threads_get(threads);

        subject = notmuch_thread_get_subject(thread);
        dateTime = relativeTime(notmuch_thread_get_newest_date(thread));

        std::ostringstream messageCountsStream;
        messageCountsStream << '[' <<
            notmuch_thread_get_matched_messages(thread) << '/' <<
            notmuch_thread_get_total_messages(thread) << ']';
        messageCounts = messageCountsStream.str();

        subject.resize(getmaxx(_window) - 21, ' ');
        dateTime.resize(13, ' ');
        messageCounts.resize(8, ' ');
        messageCounts.replace(7, 1, 1, ' ');

        if (row == _selectedRow) wattron(_window, A_REVERSE);
        wattron(_window, A_BOLD | COLOR_PAIR(NER_COLOR_YELLOW));
        mvwaddstr(_window, row, 0, dateTime.c_str());
        wattroff(_window, A_BOLD | COLOR_PAIR(NER_COLOR_YELLOW));

        mvwaddch(_window, row, 13, '[');
        wattron(_window, COLOR_PAIR(NER_COLOR_CYAN));
        wprintw(_window, "%u/%u",
            notmuch_thread_get_matched_messages(thread),
            notmuch_thread_get_total_messages(thread));
        wattroff(_window, COLOR_PAIR(NER_COLOR_CYAN));
        waddch(_window, ']');
        while (getcurx(_window) < 21) waddch(_window, ' ');

        mvwaddstr(_window, row, 21, subject.c_str());
        if (row == _selectedRow) wattroff(_window, A_REVERSE);

        notmuch_thread_destroy(thread);
    }

    notmuch_threads_destroy(threads);
}

void SearchView::handleKeyPress(const int key)
{
    switch (key)
    {
        case KEY_DOWN:
        case 'j':
            nextThread();
            break;
        case KEY_UP:
        case 'k':
            previousThread();
            break;
        case KEY_NPAGE:
        case 'd' - 'a' + 1: // Ctrl-d
            nextPage();
            break;
        case KEY_PPAGE:
        case 'u' - 'a' + 1: // Ctrl-u
            previousPage();
            break;
        case 10:
        {
            uint32_t index;
            notmuch_threads_t * threads;

            for (index = 0, threads = notmuch_query_search_threads(_query);
                index < _selectedRow + _offset && notmuch_threads_valid(threads);
                ++index, notmuch_threads_move_to_next(threads));

            _viewManager->addView(new ThreadView(
                notmuch_thread_get_thread_id(notmuch_threads_get(threads))
            ));

            notmuch_threads_destroy(threads);
            break;
        }
    }
}

void SearchView::nextThread()
{
    if (_selectedRow == getmaxy(_window) - 1)
        ++_offset;
    else
        ++_selectedRow;
}

void SearchView::previousThread()
{
    if (_selectedRow == 0)
    {
        if (_offset > 0)
            --_offset;
    }
    else
        --_selectedRow;
}

void SearchView::nextPage()
{
    _offset += getmaxy(_window);
}

void SearchView::previousPage()
{
    if (_offset < getmaxy(_window))
    {
        _offset = 0;
        _selectedRow = 0;
    }
    else
        _offset -= getmaxy(_window);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

