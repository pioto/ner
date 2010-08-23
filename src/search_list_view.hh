/* ner: src/search_list_view.hh
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

#ifndef NER_SEARCH_LIST_VIEW_H
#define NER_SEARCH_LIST_VIEW_H 1

#include <vector>

#include "line_browser_view.hh"

class SearchListView : public LineBrowserView
{
    struct Search
    {
        std::string name;
        std::string terms;
    };

    public:
        SearchListView(const View::Geometry & geometry = View::Geometry());
        virtual ~SearchListView();

        virtual void update();
        virtual std::string name() const { return "search-list-view"; }
        virtual Type type() const { return Type::SearchListView; }
        virtual std::vector<std::string> status() const;

        void openSelectedSearch();

    protected:
        virtual int lineCount() const;

    private:
        std::vector<Search> _searches;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

