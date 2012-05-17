/* ner: src/message_view.cc
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

#include "message_view.hh"
#include "notmuch.hh"


MessageView::MessageView(const View::Geometry & geometry)
    : EmailView(geometry)
{
}

MessageView::~MessageView()
{
}

void MessageView::setMessage(const std::string & messageId)
{
    Message & message = Notmuch::getMessage(messageId);
    setEmail(message.filename);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

