/* ner: src/ner.cc
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
#include <ncursesw/ncurses.h>

#include "ner.hh"
#include "util.hh"
#include "status_bar.hh"
#include "view_manager.hh"
#include "search_view.hh"
#include "message_view.hh"
#include "thread_view.hh"
#include "colors.hh"

Ner::Ner()
    : _viewManager(new ViewManager)
{
    initializeScreen();

    _statusBar = new StatusBar;

    /* Key Sequences */
    addHandledSequence("Q", std::bind(&Ner::quit, this));
    addHandledSequence("s", std::bind(&Ner::search, this));
    addHandledSequence("m", std::bind(&Ner::openMessage, this));
    addHandledSequence("t", std::bind(&Ner::openThread, this));
    addHandledSequence('l' - 96, std::bind(&Ner::redraw, this)); // Ctrl-L

    /* Colors */
    init_pair(Colors::CUT_OFF_INDICATOR,        COLOR_GREEN,    COLOR_BLACK);
    init_pair(Colors::MORE_LESS_INDICATOR,      COLOR_BLACK,    COLOR_GREEN);
    init_pair(Colors::EMPTY_SPACE_INDICATOR,    COLOR_CYAN,     COLOR_BLACK);
}

Ner::~Ner()
{
    delete _viewManager;
    delete _statusBar;
    cleanupScreen();
}

void Ner::initializeScreen()
{
    /* Initialize the screen */
    initscr();

    /* Initialize colors */
    if (has_colors())
    {
        start_color();
    }

    /* Enable raw input */
    raw();

    /* Do not echo input */
    noecho();

    /* Enable keyboard mapping */
    keypad(stdscr, TRUE);

    /* Make the cursor invisible */
    curs_set(0);
    refresh();
}

void Ner::cleanupScreen()
{
    endwin();
}

void Ner::run()
{
    std::vector<int> sequence;

    _running = true;

    _viewManager->refresh();

    while (_running)
    {
        int key = getch();

        if (key == KEY_BACKSPACE && sequence.size() > 0)
            sequence.pop_back();
        else if (key == 'c' - 96) // Ctrl-C
            sequence.clear();
        else if (key == KEY_RESIZE)
        {
            endwin();
            refresh();

            _viewManager->resize();
            _statusBar->resize();

            _viewManager->update();
            _statusBar->update();

            _viewManager->refresh();
            _statusBar->refresh();

            /* Clear the -1 character */
            getch();

            continue;
        }
        else
        {
            sequence.push_back(key);

            auto handleResult = handleKeySequence(sequence);

            /* If Ner handled the input sequence */
            if (handleResult == InputHandler::HANDLED)
                sequence.clear();
            else
            {
                auto viewManagerHandleResult = _viewManager->handleKeySequence(sequence);

                /* If the ViewManager handled the input sequence, or neither
                 * Ner nor the ViewManager had a partial match with the input
                 * sequence */
                if (viewManagerHandleResult == InputHandler::HANDLED ||
                    (viewManagerHandleResult == InputHandler::NO_MATCH &&
                        handleResult == InputHandler::NO_MATCH))
                    sequence.clear();
            }
        }

        if (!_running)
            break;

        _viewManager->update();
        _viewManager->refresh();
    }
}

void Ner::quit()
{
    _running = false;
}

void Ner::search()
{
    std::string searchTerms = StatusBar::instance().prompt("Search: ");

    if (!searchTerms.empty())
        _viewManager->addView(std::shared_ptr<SearchView>(new SearchView(searchTerms)));
}

void Ner::openMessage()
{
    std::string messageId = StatusBar::instance().prompt("Message ID: ");

    if (!messageId.empty())
    {
        std::shared_ptr<MessageView> messageView(MessageView::fromId(messageId));

        if (messageView.get())
            _viewManager->addView(messageView);
    }
}

void Ner::openThread()
{
    std::string threadId = StatusBar::instance().prompt("Thread ID: ");

    if (!threadId.empty())
    {
        std::shared_ptr<ThreadView> threadView(ThreadView::fromId(threadId));

        if (threadView.get())
            _viewManager->addView(threadView);
    }
}

void Ner::redraw()
{
    clear();
    refresh();

    _statusBar->update();
    _statusBar->refresh();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

