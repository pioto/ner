/* ner: src/view_view.cc
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

#include "view_view.hh"
#include "view_manager.hh"
#include "ncurses.hh"

#include <sstream>
#include <iterator>

const int nameWidth = 25;

ViewView::ViewView(const View::Geometry & geometry)
    : LineBrowserView(geometry)
{
    /* Colors */
    init_pair(Colors::VIEW_VIEW_NUMBER, COLOR_CYAN,     COLOR_BLACK);
    init_pair(Colors::VIEW_VIEW_NAME,   COLOR_GREEN,    COLOR_BLACK);
    init_pair(Colors::VIEW_VIEW_STATUS, COLOR_WHITE,    COLOR_BLACK);

    /* Key Sequences */
    addHandledSequence("\n", std::bind(&ViewView::openSelectedView, this));
    addHandledSequence("x", std::bind(&ViewView::closeSelectedView, this));
}

ViewView::~ViewView()
{
}

void ViewView::update()
{
    werase(_window);

    if (_offset > lineCount())
        return;

    int row = 0;

    for (auto view = ViewManager::instance()._views.begin() + _offset,
        e = ViewManager::instance()._views.end();
        view != e && row < getmaxy(_window); ++view, ++row)
    {
        /* Don't list the ViewView */
        if ((*view)->type() == View::Type::ViewView)
            continue;

        bool selected = row + _offset == _selectedIndex;

        int x = 0;

        wmove(_window, row, x);

        attr_t attributes = 0;

        if (selected)
        {
            attributes |= A_REVERSE;
            wchgat(_window, -1, A_REVERSE, 0, NULL);
        }

        /* Number */
        std::ostringstream numberStream;
        numberStream << row + _offset << ".";
        x += NCurses::addPlainString(_window, numberStream.str(),
            attributes, Colors::VIEW_VIEW_NUMBER);

        NCurses::checkMove(_window, ++x);

        /* Name */
        NCurses::addPlainString(_window, (*view)->name(),
            attributes, Colors::VIEW_VIEW_NAME, nameWidth - 1);

        NCurses::checkMove(_window, x = nameWidth);

        /* Status */
        std::vector<std::string> status((*view)->status());
        if (status.size() > 0)
            NCurses::addPlainString(_window, status.at(0), attributes, Colors::VIEW_VIEW_STATUS);
    }
}

void ViewView::unfocus()
{
    /* Close the active view (this one). We don't want multiple instances of
     * ViewView around */
    ViewManager::instance().closeActiveView();
}

int ViewView::lineCount() const
{
    return ViewManager::instance()._views.size() - 1;
}

void ViewView::openSelectedView()
{
    ViewManager::instance().openView(_selectedIndex);
}

void ViewView::closeSelectedView()
{
    /* Decrement by one to account for ViewView */
    int views = ViewManager::instance()._views.size() - 1;

    if (views > 1)
    {
        ViewManager::instance().closeView(_selectedIndex);
        _selectedIndex = std::min(_selectedIndex, views - 2);
    }
    else
        StatusBar::instance().displayMessage("This is the last view left, use Q to quit");
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

