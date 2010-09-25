/* ner: src/line_wrapper.cc
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

#include "line_wrapper.hh"

LineWrapper::LineWrapper(const std::string & string, int width)
    : _start(string.begin()), _position(string.begin()), _end(string.end()),
        _width(width), _done(false)
{
}

std::string LineWrapper::next()
{
    std::string line;

    if (_position + _width < _end)
    {
        auto notSpace = std::bind(std::logical_not<bool>(),
            std::bind(std::equal_to<char>(), ' ', std::placeholders::_1));

        auto lineEnd = std::find_if(std::find(
            std::string::const_reverse_iterator(_position + _width + 1),
            std::string::const_reverse_iterator(_position), ' '),
            std::string::const_reverse_iterator(_position), notSpace).base();

        if (lineEnd == _position && (lineEnd = std::find(_position + _width, _end, ' ')) == _end)
            _done = true;

        line = std::string(_position, lineEnd);

        _position = std::find_if(lineEnd, _end, notSpace);
    }
    else
    {
        line = std::string(_position, _end);
        _position = _end;
        _done = true;
    }

    return line;
}

bool LineWrapper::done() const
{
    return _done;
}

bool LineWrapper::wrapped() const
{
    return _position != _start;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

