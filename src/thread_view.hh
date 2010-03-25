/* ner: src/thread_view.hh
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

#ifndef NER_THREAD_VIEW_H
#define NER_THREAD_VIEW_H 1

#include <notmuch.h>
#include <vector>
#include <unordered_map>

#include "line_browser_view.hh"

class ThreadView : public LineBrowserView
{
    struct Message
    {
        Message() = default;
        Message(notmuch_message_t * message);

        std::string id;
        std::string filename;
        time_t date;
        bool matched;
        std::map<std::string, std::string> headers;
        std::vector<Message> replies;
    };

    public:
        ThreadView(const std::string & id);
        virtual ~ThreadView();

        virtual void update();
        virtual std::string name() const { return "thread-view"; }

        virtual void openSelectedMessage();

    protected:
        virtual int lineCount() const;
        const Message & selectedMessage() const;

    private:
        uint32_t displayMessageLine(const Message & message,
            std::vector<chtype> & leading, bool last, int index);

        Message _topMessage;
        int _messageCount;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

