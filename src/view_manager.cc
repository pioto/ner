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

#include <algorithm>

#include "view_manager.hh"
#include "view.hh"
#include "view_view.hh"
#include "status_bar.hh"

ViewManager * ViewManager::_instance = 0;

ViewManager::ViewManager()
{
    _instance = this;

    addHandledSequence("q", std::bind(&ViewManager::closeActiveView, this));
}

ViewManager::~ViewManager()
{
}

InputHandler::HandleResult ViewManager::handleKeySequence(const std::vector<int> & sequence)
{
    auto handleResult = InputHandler::handleKeySequence(sequence);

    if (handleResult == InputHandler::HandleResult::Handled)
        return InputHandler::HandleResult::Handled;
    else
    {
        auto activeViewHandleResult = _activeView->handleKeySequence(sequence);

        if (activeViewHandleResult == InputHandler::HandleResult::Handled)
            return InputHandler::HandleResult::Handled;
        else if (handleResult == InputHandler::HandleResult::NoMatch &&
            activeViewHandleResult == InputHandler::HandleResult::NoMatch)
            return InputHandler::HandleResult::NoMatch;
        else
            return InputHandler::HandleResult::PartialMatch;
    }
}

void ViewManager::addView(const std::shared_ptr<View> & view)
{
    if (_activeView.get())
        _activeView->unfocus();

    _views.push_back(view);
    _activeView = view;

    _activeView->focus();
    _activeView->update();
    _activeView->refresh();

    StatusBar::instance().update();
    StatusBar::instance().refresh();
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
        _views.erase(std::find(_views.begin(), _views.end(), _activeView));

        _activeView = _views.back();

        _activeView->focus();

        StatusBar::instance().update();
        StatusBar::instance().refresh();
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

void ViewManager::resize()
{
    for (auto view = _views.begin(), e = _views.end(); view != e; ++view)
    {
        (*view)->resize();
    }
}

const View & ViewManager::activeView() const
{
    return *_activeView;
}

void ViewManager::openView(int index)
{
    _activeView->unfocus();

    _activeView = _views.at(index);

    StatusBar::instance().update();
    StatusBar::instance().refresh();

    _activeView->focus();

    _activeView->update();
    _activeView->refresh();
}

void ViewManager::closeView(int index)
{
    /* If this is our last view */
    if (_views.size() == 1)
        StatusBar::instance().displayMessage("This is the last view left, use Q to quit");
    else
    {
        auto view = _views.begin() + index;

        if (_activeView == *view)
        {
            if (index < _views.size() - 1)
                _activeView = *(view + 1);
            else
                _activeView = *(view - 1);
        }

        _views.erase(view);
    }
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

