/* ner: src/search_view.hh
 *
 * Copyright (c) 2010 Michael Forney
 *
 * This file is a part of ner.
 *
 * ner is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2, as published by the Free
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
#include <notmuch.h>

#include "window_view.hh"
#include "notmuch.hh"

class SearchView : public WindowView
{
    public:
        SearchView(const std::string & search);
        ~SearchView();

        virtual void update();
        virtual void handleKeyPress(const int key);

        void nextThread();
        void previousThread();
        void nextPage();
        void previousPage();

    private:
        notmuch_query_t * _query;
        uint16_t _selectedRow;
        uint64_t _offset;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

