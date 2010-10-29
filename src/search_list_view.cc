/* ner: src/search_list_view.cc
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

#include <sstream>

#include "search_list_view.hh"
#include "view_manager.hh"
#include "search_view.hh"
#include "ncurses.hh"
#include "ner_config.hh"

const int searchNameWidth = 15;
const int searchTermsWidth = 30;

SearchListView::SearchListView(const View::Geometry & geometry)
    : LineBrowserView(geometry),
        _searches(NerConfig::instance().searches())
{
    /* Key Sequences */
    addHandledSequence("\n", std::bind(&SearchListView::openSelectedSearch, this));
}

SearchListView::~SearchListView()
{
}

void SearchListView::update()
{
    werase(_window);

    if (_offset > _searches.size())
        return;

    int row = 0;

    for (auto search = _searches.begin();
        search != _searches.end() && row < getmaxy(_window);
        ++search, ++row)
    {
        bool selected = row + _offset == _selectedIndex;

        int x = 0;

        wmove(_window, row, x);

        attr_t attributes = 0;

        if (selected)
            attributes |= A_REVERSE;

        wchgat(_window, -1, attributes, 0, NULL);

        try
        {
            /* Search Name */
            NCurses::addUtf8String(_window, search->name.c_str(), attributes,
                ColorID::SearchListViewName, searchNameWidth - 1);

            NCurses::checkMove(_window, x += searchNameWidth);

            /* Search Terms */
            NCurses::addUtf8String(_window, search->query.c_str(), attributes,
                ColorID::SearchListViewTerms, searchTermsWidth - 1);

            NCurses::checkMove(_window, x += searchTermsWidth);

            /* Number of Results */
            std::ostringstream results;
            notmuch_database_t * database = NotMuch::openDatabase();
            notmuch_query_t * query = notmuch_query_create(database, search->query.c_str());
            results << notmuch_query_count_messages(query) << " results";
            notmuch_query_destroy(query);
            notmuch_database_close(database);

            NCurses::addPlainString(_window, results.str(), attributes,
                ColorID::SearchListViewResults);

            NCurses::checkMove(_window, x - 1);
        }
        catch (const NCurses::CutOffException & e)
        {
            NCurses::addCutOffIndicator(_window, attributes);
        }
    }
}

std::vector<std::string> SearchListView::status() const
{
    std::ostringstream searchPosition;

    if (_searches.size() > 0)
        searchPosition << "search " << (_selectedIndex + 1) << " of " <<
            _searches.size();
    else
        searchPosition << "no configured searches";

    return std::vector<std::string>{ searchPosition.str() };
}

int SearchListView::lineCount() const
{
    return _searches.size();
}

void SearchListView::openSelectedSearch()
{
    ViewManager::instance().addView(std::make_shared<SearchView>(
        _searches.at(_selectedIndex).query));
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

