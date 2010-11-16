/* ner: src/line_editor.hh
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

#ifndef NER_LINE_EDITOR_H
#define NER_LINE_EDITOR_H 1

#include <map>
#include <vector>

#include "ncurses.hh"

class AbortInputException : public std::exception
{
};

class LineEditor
{
    public:
        LineEditor(WINDOW * window, int x, int y);

        std::string line(const std::string & field = std::string()) const;

    private:
        WINDOW * _window;
        int _x;
        int _y;

        static std::map<std::string, std::vector<std::string>> _history;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

