/* ner: src/status_bar.cc
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

#include <ncurses.h>

#include "status_bar.hh"

StatusBar * StatusBar::_instance = 0;

StatusBar::StatusBar()
{
    _instance = this;
}

StatusBar::~StatusBar()
{
    _instance = 0;
}

void StatusBar::displayMessage(const std::string & message)
{
    mvaddnstr(LINES - 1, 0, message.c_str(), 50);
}

std::string StatusBar::prompt(const std::string & message)
{
    char response[256];

    displayMessage(message);
    echo();
    curs_set(1);
    getnstr(response, sizeof(response));
    curs_set(0);
    noecho();
    move(LINES - 1, 0);
    clrtoeol();

    return std::string(response);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8


