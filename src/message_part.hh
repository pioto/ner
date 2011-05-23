/* ner: src/message_part.hh
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

#ifndef NER_MESSAGE_PART_H
#define NER_MESSAGE_PART_H 1

#include <string>
#include <vector>
#include <gmime/gmime.h>

#include "ncurses.hh"
#include "view.hh"

class MessagePartVisitor;

struct MessagePart
{
    MessagePart(const std::string & id_);

    virtual void accept(MessagePartVisitor & visitor) = 0;

    bool folded;
    std::string id;
};

struct TextPart : public MessagePart
{
    TextPart(GMimePart * part);

    virtual void accept(MessagePartVisitor & visitor);

    std::vector<std::string> lines;
};

struct Attachment : public MessagePart
{
    Attachment(GMimePart * part);
    Attachment(GMimeDataWrapper * data, const std::string & filename,
               const std::string& contentType, int filesize);
    ~Attachment();

    virtual void accept(MessagePartVisitor & visitor);

    std::string filename;
    std::string contentType;
    int filesize;
    GMimeDataWrapper * data;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

