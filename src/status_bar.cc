/* ner: src/status_bar.cc
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

#include <ncurses.h>

#include "status_bar.hh"
#include "colors.hh"

StatusBar * StatusBar::_instance = 0;

const int viewNameWidth = 15;
const int dividerWidth = 3;

StatusBar::StatusBar()
    : _statusWindow(newwin(1, COLS, LINES - 2, 0)),
        _promptWindow(newwin(1, COLS, LINES - 1, 0)),
        _messageCleared(true)
{
    _instance = this;

    /* Colors */
    init_pair(Colors::STATUS_BAR_STATUS,            COLOR_WHITE,    COLOR_BLUE);
    init_pair(Colors::STATUS_BAR_MESSAGE,           COLOR_BLACK,    COLOR_WHITE);
    init_pair(Colors::STATUS_BAR_PROMPT,            COLOR_WHITE,    COLOR_BLACK);

    wbkgd(_statusWindow, COLOR_PAIR(Colors::STATUS_BAR_STATUS));

    wrefresh(_statusWindow);
    wrefresh(_promptWindow);
}

StatusBar::~StatusBar()
{
    _instance = 0;

    if (_messageClearThread.joinable())
        _messageClearThread.join();
}

void StatusBar::update()
{
    werase(_statusWindow);
    wmove(_statusWindow, 0, 0);

    /* View Name */
    wattron(_statusWindow, A_BOLD);
    waddch(_statusWindow, '[');
    waddnstr(_statusWindow, _viewName.c_str(), viewNameWidth - 2 - 1);
    waddch(_statusWindow, ']');

    /* Divider */
    wmove(_statusWindow, 0, viewNameWidth);
    waddstr(_statusWindow, " | ");
    wattroff(_statusWindow, A_BOLD);

    /* Status */
    wmove(_statusWindow, 0, viewNameWidth + dividerWidth);
    waddstr(_statusWindow, _status.c_str());

    wrefresh(_statusWindow);
}

void StatusBar::refresh()
{
    wrefresh(_statusWindow);
    wrefresh(_promptWindow);
}

void StatusBar::resize()
{
    wresize(_statusWindow, 1, COLS);
    wresize(_promptWindow, 1, COLS);

    mvwin(_statusWindow, LINES - 2, 0);
    mvwin(_promptWindow, LINES - 1, 0);
}

void StatusBar::displayMessage(const std::string & message)
{
    werase(_promptWindow);
    wbkgd(_promptWindow, COLOR_PAIR(Colors::STATUS_BAR_MESSAGE));

    wmove(_promptWindow, 0, (getmaxx(_promptWindow) - message.size()) / 2);
    wattron(_promptWindow, A_BOLD);
    waddstr(_promptWindow, message.c_str());
    wattroff(_promptWindow, A_BOLD);

    wrefresh(_promptWindow);

    _messageCleared = false;

    if (_messageClearThread.joinable())
        _messageClearThread.detach();

    _messageClearThread = std::thread(std::bind(&StatusBar::delayedClearMessage, this, 1500));
}

std::string StatusBar::prompt(const std::string & message)
{
    char response[256];

    if (!_messageCleared)
        clearMessage();

    wmove(_promptWindow, 0, 0);
    wattron(_promptWindow, COLOR_PAIR(Colors::STATUS_BAR_PROMPT));
    waddstr(_promptWindow, message.c_str());
    echo();
    curs_set(1);
    wgetnstr(_promptWindow, response, sizeof(response));
    curs_set(0);
    noecho();
    wattroff(_promptWindow, COLOR_PAIR(Colors::STATUS_BAR_PROMPT));

    /* Clear the prompt window after we're done */
    werase(_promptWindow);

    return std::string(response);
}

void StatusBar::setViewName(const std::string & name)
{
    _viewName = name;

    update();
}

void StatusBar::setStatus(const std::string & status)
{
    _status = status;

    update();
}

void StatusBar::delayedClearMessage(int delay)
{
    usleep(delay * 1000);

    if (_messageCleared)
        return;

    clearMessage();
}

void StatusBar::clearMessage()
{
    werase(_promptWindow);
    wbkgd(_promptWindow, COLOR_PAIR(Colors::STATUS_BAR_PROMPT));
    wrefresh(_promptWindow);
    _messageCleared = true;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

