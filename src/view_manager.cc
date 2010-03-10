/* ner: src/view_manager.cc
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

#include "view_manager.hh"
#include "view.hh"
#include "status_bar.hh"

ViewManager::ViewManager()
    : _activeView(0)
{
}

ViewManager::~ViewManager()
{
}

void ViewManager::handleKeyPress(const int key)
{
    switch (key)
    {
        case 'q':
            closeActiveView();
            break;
        default:
            _activeView->handleKeyPress(key);
    }
}

void ViewManager::addView(View * view)
{
    view->_viewManager = this;

    _views.push_back(view);

    _activeView = view;

    _activeView->update();
    _activeView->refresh();
}

void ViewManager::closeActiveView()
{
    /* If this is our last view */
    if (_views.size() == 1)
    {
        StatusBar::instance().displayMessage("This is the last view left, use Q to quit");
    }
    else
    {
        delete _views.back();
        _views.pop_back();
        _activeView = _views.back();
        _activeView->focus();
        _activeView->update();
        _activeView->refresh();
    }
}

void ViewManager::update()
{
    _activeView->update();
}

void ViewManager::refresh()
{
    _activeView->refresh();
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8


