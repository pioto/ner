/* ner: src/line_editor.cc
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

#include <functional>
#include <iostream>

#include "line_editor.hh"
#include "ncurses.hh"

std::map<std::string, std::vector<std::string>> LineEditor::_history;

LineEditor::LineEditor(WINDOW * window, int x, int y)
    : _window(window), _x(x), _y(y)
{
}

std::string LineEditor::line(const std::string & field) const
{
    std::vector<std::string> history;

    if (!field.empty())
        history = _history[field];

    history.push_back(std::string());

    auto response = history.rbegin();
    auto position = response->begin();

    wmove(_window, _x, _y);

    curs_set(1);

    int c;

    while ((c = getch()) != '\n')
    {
        switch (c)
        {
            case KEY_LEFT:
                if (position > response->begin())
                    --position;
                break;
            case KEY_RIGHT:
                if (position < response->end())
                    ++position;
                break;
            case KEY_SLEFT:
                /* Navigate to the beginning of the previous word from the cursor.
                 *
                 * This is the character after the first space before the first
                 * character that is not a space at or before the cursor
                 * position. */
                position = response->begin() + (response->rend() - std::find(std::find_if(
                    response->rbegin() + (response->end() - position) + 1, response->rend(),
                    std::bind(std::logical_not<bool>(), std::bind(std::equal_to<char>(),
                    ' ', std::placeholders::_1))), response->rend(), ' '));
                break;
            case KEY_SRIGHT:
                /* Navigate to the beginning of next word from the cursor.
                 *
                 * This is the first charactor that is not a space, starting
                 * from the first space at or after the cursor position. */
                position = std::find_if(std::find(position, response->end(), ' '),
                    response->end(), std::bind(std::logical_not<bool>(),
                    std::bind(std::equal_to<char>(), ' ', std::placeholders::_1)));
                break;
            case KEY_UP:
                if (response < history.rend() - 1)
                {
                    ++response;
                    position = response->end();
                }
                break;
            case KEY_DOWN:
                if (response > history.rbegin())
                {
                    --response;
                    position = response->end();
                }
                break;
            case 'h' - 96:
            case KEY_BACKSPACE:
            case 127:
                if (position > response->begin())
                    position = response->erase(position - 1);
                break;
            case KEY_DC:
                if (position < response->end())
                    position = response->erase(position);
                break;
            case KEY_HOME:
                position = response->begin();
                break;
            case KEY_END:
                position = response->end();
                break;
            case '\t':
                /* TODO: Auto complete */
                break;
            case 'u' - 96:
                position = response->erase(response->begin(), position);
                break;
            case 'w' - 96:
                /* Erase all characters from the beginning of the previous word
                 * (see KEY_SLEFT handler) to the cursor position. */
                position = response->erase(response->begin() + (response->rend() -
                    std::find(std::find_if(response->rbegin() + (response->end() -
                    position) + 1, response->rend(), std::bind(std::logical_not<bool>(),
                    std::bind(std::equal_to<char>(), ' ', std::placeholders::_1))),
                    response->rend(), ' ')), position);
                break;
            default:
                position = response->insert(position, c) + 1;
        }

        wmove(_window, _y, _x);
        wclrtoeol(_window);
        waddstr(_window, response->c_str());
        wmove(_window, _y, _x + (position - response->begin()));
        wrefresh(_window);
    }

    curs_set(0);

    if (!field.empty() && !response->empty())
        _history[field].push_back(*response);

    return *response;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

