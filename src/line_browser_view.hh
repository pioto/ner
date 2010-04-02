/* ner: src/line_browser_view.hh
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

#ifndef NER_LINE_BROWSER_VIEW_H
#define NER_LINE_BROWSER_VIEW_H 1

#include "window_view.hh"

class LineBrowserView : public WindowView
{
    public:
        LineBrowserView(int x, int y, int width, int height);

        virtual void resize(int x, int y, int width, int height);

        virtual void next();
        virtual void previous();
        virtual void nextPage();
        virtual void previousPage();
        virtual void moveToTop();
        virtual void moveToBottom();

    protected:
        virtual int visibleLines() const;
        virtual int lineCount() const = 0;
        virtual void makeSelectionVisible();

        int _offset;
        int _selectedIndex;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

