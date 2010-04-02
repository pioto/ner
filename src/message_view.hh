/* ner: src/message_view.hh
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

#ifndef NER_MESSAGE_VIEW_H
#define NER_MESSAGE_VIEW_H 1

#include <string>
#include <vector>
#include <gmime/gmime.h>
#include <notmuch.h>

#include "line_browser_view.hh"

class MessageView : public LineBrowserView
{
    public:
        MessageView(int x = defaultX(), int y = defaultY(),
            int width = defaultWidth(), int height = defaultHeight());
        virtual ~MessageView();

        static MessageView * fromId(const std::string & messageId);

        void setMessage(notmuch_message_t * message);

        virtual void update();
        virtual std::string name() const { return "message-view"; }
        virtual std::vector<std::string> status() const;

    protected:
        virtual int visibleLines() const;
        virtual int lineCount() const;

        void processMimePart(GMimeObject * part);

        std::map<std::string, std::string> _headers;
        std::vector<std::string> _lines;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

