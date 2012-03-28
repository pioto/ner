/* ner: src/message_part_display_visitor.hh
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

#ifndef NER_MESSAGE_PART_DISPLAY_VISITOR_H
#define NER_MESSAGE_PART_DISPLAY_VISITOR_H 1

#include "message_part_visitor.hh"
#include "ncurses.hh"
#include "view.hh"

class MessagePartDisplayVisitor : public MessagePartVisitor
{
    public:
        MessagePartDisplayVisitor(WINDOW * window, const View::Geometry & area,
            int offset, int selection, bool displayPartName);

        virtual void visit(const TextPart & part);
        virtual void visit(const Attachment & part);

        int row() const;
        int lines() const;

    private:
        WINDOW * _window;
        View::Geometry _area;
        int _row;
        int _messageRow;
        int _offset;
        int _selection;

        bool _displayPartName;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

