/* ner: src/line_browser_view.cc
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

#include "line_browser_view.hh"

LineBrowserView::LineBrowserView(int x, int y, int width, int height)
    : WindowView(x, y, width, height),
        _selectedIndex(0),
        _offset(0)
{
    /* Key Sequences */
    addHandledSequence("j",         std::bind(&LineBrowserView::next, this));
    addHandledSequence(KEY_DOWN,    std::bind(&LineBrowserView::next, this));
    addHandledSequence("k",         std::bind(&LineBrowserView::previous, this));
    addHandledSequence(KEY_UP,      std::bind(&LineBrowserView::previous, this));

    addHandledSequence(KEY_NPAGE,   std::bind(&LineBrowserView::nextPage, this));
    addHandledSequence('d' - 96,    std::bind(&LineBrowserView::nextPage, this)); // Ctrl-D
    addHandledSequence(KEY_PPAGE,   std::bind(&LineBrowserView::previousPage, this));
    addHandledSequence('u' - 96,    std::bind(&LineBrowserView::previousPage, this)); // Ctrl-U

    addHandledSequence("gg",        std::bind(&LineBrowserView::moveToTop, this));
    addHandledSequence(KEY_HOME,    std::bind(&LineBrowserView::moveToTop, this));
    addHandledSequence("G",         std::bind(&LineBrowserView::moveToBottom, this));
    addHandledSequence(KEY_END,     std::bind(&LineBrowserView::moveToBottom, this));
}

void LineBrowserView::resize(int x, int y, int width, int height)
{
    WindowView::resize(x, y, width, height);

    makeSelectionVisible();
}

std::vector<std::string> LineBrowserView::status() const
{
    std::ostringstream position;

    if (lineCount() > 0)
        position << "line " << _selectedIndex + 1 << " of " << lineCount();
    else
        position << "no lines";

    return std::vector<std::string>{ position.str() };
}

void LineBrowserView::next()
{
    if (_selectedIndex + 1 < lineCount())
        ++_selectedIndex;

    makeSelectionVisible();
    updateStatus();
}

void LineBrowserView::previous()
{
    if (_selectedIndex > 0)
        --_selectedIndex;

    makeSelectionVisible();
    updateStatus();
}

void LineBrowserView::nextPage()
{
    if (_selectedIndex + visibleLines() >= lineCount())
        _selectedIndex = lineCount() - 1;
    else
        _selectedIndex += visibleLines() - 1;

    makeSelectionVisible();
    updateStatus();
}

void LineBrowserView::previousPage()
{
    if (getmaxy(_window) > _selectedIndex)
        _selectedIndex = 0;
    else
        _selectedIndex -= visibleLines() - 1;

    makeSelectionVisible();
    updateStatus();
}

void LineBrowserView::moveToTop()
{
    _selectedIndex = 0;

    makeSelectionVisible();
    updateStatus();
}

void LineBrowserView::moveToBottom()
{
    _selectedIndex = lineCount() - 1;

    makeSelectionVisible();
    updateStatus();
}

void LineBrowserView::makeSelectionVisible()
{
    if (_selectedIndex < _offset)
        _offset = _selectedIndex;
    else if (_selectedIndex >= _offset + visibleLines())
        _offset = _selectedIndex - visibleLines() + 1;
}

int LineBrowserView::visibleLines() const
{
    return getmaxy(_window);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

