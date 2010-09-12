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

#include <ncursesw/ncurses.h>

#include "status_bar.hh"
#include "colors.hh"
#include "ncurses.hh"
#include "view.hh"
#include "view_manager.hh"

StatusBar * StatusBar::_instance = 0;

StatusBar::StatusBar()
    : _statusWindow(newwin(1, COLS, LINES - 2, 0)),
        _promptWindow(newwin(1, COLS, LINES - 1, 0)),
        _messageCleared(true)
{
    _instance = this;

    wbkgd(_statusWindow, COLOR_PAIR(ColorID::StatusBarStatus));

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
    int x = 0;

    werase(_statusWindow);
    wmove(_statusWindow, 0, x);

    const View & view = ViewManager::instance().activeView();

    /* View Name */
    x += NCurses::addPlainString(_statusWindow, '[' + view.name() + ']',
        A_BOLD, ColorID::StatusBarStatus);

    /* Status */
    std::vector<std::string> status(view.status());
    for (auto statusItem = status.begin(), e = status.end(); statusItem != e; ++statusItem)
    {
        try
        {
            /* Divider */
            NCurses::checkMove(_statusWindow, ++x);

            x += NCurses::addChar(_statusWindow, '|', A_BOLD, ColorID::StatusBarStatusDivider);

            NCurses::checkMove(_statusWindow, ++x);

            x += NCurses::addPlainString(_statusWindow, *statusItem, 0, ColorID::StatusBarStatus);
        }
        catch (const NCurses::CutOffException & e)
        {
        }
    }
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
    wbkgd(_promptWindow, COLOR_PAIR(ColorID::StatusBarMessage));

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
    wattron(_promptWindow, COLOR_PAIR(ColorID::StatusBarPrompt));
    waddstr(_promptWindow, message.c_str());
    echo();
    curs_set(1);
    wgetnstr(_promptWindow, response, sizeof(response));
    curs_set(0);
    noecho();
    wattroff(_promptWindow, COLOR_PAIR(ColorID::StatusBarPrompt));

    /* Clear the prompt window after we're done */
    werase(_promptWindow);
    wrefresh(_promptWindow);

    return std::string(response);
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
    wbkgd(_promptWindow, COLOR_PAIR(ColorID::StatusBarPrompt));
    wrefresh(_promptWindow);
    _messageCleared = true;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

