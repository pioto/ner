/* ner: src/message_part_display_visitor.cc
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

#include <sstream>

#include "message_part_display_visitor.hh"
#include "colors.hh"
#include "message_part.hh"
#include "line_wrapper.hh"
#include "util.hh"

const int wrapWidth(80);

MessagePartDisplayVisitor::MessagePartDisplayVisitor(WINDOW * window,
    const View::Geometry & area, int offset, int selection)
    : _window(window), _area(area), _offset(offset), _row(area.y), _messageRow(0),
        _selection(selection)
{
}

void MessagePartDisplayVisitor::visit(const TextPart & part)
{
    for (auto line = part.lines.begin(), e = part.lines.end(); line != e; ++line)
    {
        for (auto lineWrapper = LineWrapper(*line); !lineWrapper.done(); ++_messageRow)
        {
            bool selected = _messageRow == _selection;
            bool wrapped = lineWrapper.wrapped();

            std::string wrappedLine(lineWrapper.next());

            if (_messageRow < _offset || _row >= _area.y + _area.height)
                continue;

            if (wrapped)
                mvwaddch(_window, _row, _area.x, ACS_CKBOARD | COLOR_PAIR(ColorID::LineWrapIndicator));

            wmove(_window, _row, _area.x + 2);

            attr_t attributes = 0;

            if (selected)
            {
                attributes |= A_REVERSE;
                wchgat(_window, _area.width - 2, A_REVERSE, 0, NULL);
            }

            if (NCurses::addUtf8String(_window, wrappedLine.c_str(), attributes) >
                _area.width - _area.y - 2)
            {
                NCurses::addCutOffIndicator(_window, attributes);
            }

            ++_row;
        }
    }
}

void MessagePartDisplayVisitor::visit(const Attachment & part)
{
    if (_messageRow >= _offset && _row < _area.y + _area.height)
    {
        try
        {
            bool selected = _messageRow == _selection;

            int x = _area.x;

            wmove(_window, _row++, _area.x);

            attr_t attributes = 0;

            x += NCurses::addChar(_window, '*', A_BOLD | attributes, ColorID::AttachmentFilename);
            NCurses::checkMove(_window, ++x);

            if (selected)
            {
                attributes |= A_REVERSE;
                wchgat(_window, -1, A_REVERSE, 0, NULL);
            }

            x += NCurses::addPlainString(_window, "Attachment: ", attributes);
            NCurses::checkMove(_window, x);

            x += NCurses::addUtf8String(_window, part.filename.c_str(), attributes,
                ColorID::AttachmentFilename);
            NCurses::checkMove(_window, ++x);

            x += NCurses::addPlainString(_window, formatByteSize(part.filesize), attributes,
                ColorID::AttachmentFilesize);

            NCurses::checkMove(_window, x - 1);
        }
        catch (const NCurses::CutOffException & e)
        {
            NCurses::addCutOffIndicator(_window);
        }
    }

    ++_messageRow;
}

int MessagePartDisplayVisitor::row() const
{
    return _row;
}

int MessagePartDisplayVisitor::lines() const
{
    return _messageRow;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

