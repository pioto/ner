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
#include <thread>

#include "line_browser_view.hh"
#include "notmuch.hh"
#include "thread.hh"

class SearchView : public LineBrowserView
{
    public:
        SearchView(const std::string & search,
            const View::Geometry & geometry = View::Geometry());
        virtual ~SearchView();

        virtual void update();
        virtual std::string name() const { return "search-view"; }
        virtual std::vector<std::string> status() const;

        void refreshThreads();

        void openSelectedThread();
        void archiveSelectedThread();
        void toggleUnreadSelectedThread();

        void addTags();
        void removeTags();

    protected:
        virtual int lineCount() const;

    private:
        void collectThreads();

        std::string _searchTerms;

        std::thread _thread;
        std::mutex _mutex;
        std::condition_variable _condition;
        bool _collecting;

        std::vector<Thread> _threads;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

