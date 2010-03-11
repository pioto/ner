/* ner: src/search_view.hh
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

#ifndef NER_SEARCH_VIEW
#define NER_SEARCH_VIEW 1

#include <string>
#include <map>
#include <set>
#include <thread>
#include <notmuch.h>

#include "window_view.hh"
#include "notmuch.hh"

class SearchView : public WindowView
{
    struct Thread
    {
        Thread(notmuch_thread_t * thread);

        std::string id;
        std::string subject;
        std::string authors;
        uint32_t totalMessages;
        uint32_t matchedMessages;
        time_t newestDate;
        time_t oldestDate;
        std::set<std::string> tags;
    };

    class ThreadCollector
    {
        public:
            ThreadCollector();

            void start(notmuch_query_t * query);

            std::vector<Thread> threads;
            bool finished;

        private:
            void collect();

            notmuch_query_t * _query;
            std::thread _thread;
            bool _running;
    };

    public:
        SearchView(const std::string & search);
        ~SearchView();

        virtual void update();

        void nextThread();
        void previousThread();
        void nextPage();
        void previousPage();
        void moveToTop();
        void moveToBottom();
        void openSelectedThread();

    private:
        void makeSelectionVisible();
        void collectThreads();

        notmuch_query_t * _query;
        ThreadCollector _collector;
        bool _doneCollectingThreads;
        uint32_t _selectedIndex;
        uint32_t _offset;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

