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

#include "view.hh"

class ThreadView : public View
{
    struct Message
    {
        Message() = default;
        Message(notmuch_message_t * message, Message * parentMessage = 0);

        std::string id;
        std::string filename;
        time_t date;
        bool matched;
        std::unordered_map<std::string, std::string> headers;
        std::vector<Message> replies;
        Message * parent;
    };

    public:
        ThreadView(const std::string & id);
        ~ThreadView();

        virtual void update();
        virtual void refresh();
        virtual void focus();
        virtual void resize();
        virtual std::string name() const { return "thread-view"; }

    private:
        uint32_t displayMessageLine(const Message & message,
            uint32_t start, std::vector<chtype> & leading, bool last, uint32_t row);

        Message _topMessage;
        Message * _selectedMessage;
        notmuch_query_t * _query;
        notmuch_thread_t * _thread;
        WINDOW * _threadWindow;
        WINDOW * _messageWindow;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

