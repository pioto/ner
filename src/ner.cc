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
#include <ncurses.h>

#include "ner.hh"
#include "util.hh"
#include "status_bar.hh"
#include "view_manager.hh"
#include "search_view.hh"

Ner::Ner()
    : _viewManager(new ViewManager)
{
    initializeScreen();

    _statusBar = new StatusBar;

    addHandledSequence("Q", std::bind(&Ner::quit, this));
    addHandledSequence("s", std::bind(&Ner::search, this));
    addHandledSequence('l' - 96, std::bind(&Ner::redraw, this)); // Ctrl-L
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
        _viewManager->addView(new SearchView(searchTerms));
}

void Ner::redraw()
{
    clear();
    refresh();

    _statusBar->update();
    _statusBar->refresh();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

