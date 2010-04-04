/* ner: src/thread_message_view.hh
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

#ifndef NER_THREAD_MESSAGE_VIEW_H
#define NER_THREAD_MESSAGE_VIEW_H 1

#include "thread_view.hh"
#include "message_view.hh"

class ThreadMessageView : public View
{
    public:
        ThreadMessageView(const std::string & threadId,
            int x = defaultX(), int y = defaultY(),
            int width = defaultWidth(), int height = defaultHeight());
        virtual ~ThreadMessageView();

        virtual void update();
        virtual void refresh();
        virtual void resize(int x, int y, int width, int height);

        virtual std::string name() const { return "thread-message-view"; }
        virtual std::vector<std::string> status() const;

        void nextMessage();
        void previousMessage();

    protected:
        void loadSelectedMessage();

    private:
        ThreadView _threadView;
        MessageView _messageView;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

