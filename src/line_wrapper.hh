/* ner: src/line_wrapper.hh
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

#ifndef NER_LINE_WRAPPER_H
#define NER_LINE_WRAPPER_H 1

#include <string>
#include <algorithm>
#include <functional>

class LineWrapper
{
    public:
        explicit LineWrapper(const std::string & string, int width = 80);

        std::string next();
        bool done() const;
        bool wrapped() const;

    private:
        std::string::const_iterator _start;
        std::string::const_iterator _position;
        std::string::const_iterator _end;
        int _width;
        bool _done;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

