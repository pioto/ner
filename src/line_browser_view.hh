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

/**
 * A general view that browses through a number of lines
 */
class LineBrowserView : public WindowView
{
    public:
        LineBrowserView(const View::Geometry & geometry = View::Geometry());

        virtual void resize(const View::Geometry & geometry = View::Geometry());

        virtual std::vector<std::string> status() const;

        /**
         * Advances the cursor to the next line.
         */
        virtual void next();

        /**
         * Moves the cursor back to the previous line.
         */
        virtual void previous();

        /**
         * Moves the cursor down one page.
         */
        virtual void nextPage();

        /**
         * Moves the cursor up one page.
         */
        virtual void previousPage();

        /**
         * Moves the cursor to the first line
         */
        virtual void moveToTop();

        /**
         * Moves the cursor to the last line
         */
        virtual void moveToBottom();

    protected:
        /**
         * Returns the number of lines visible on the screen.
         *
         * This should be reimplemented when for line browsers that are not full
         * screen.
         *
         * \return The number of lines on the screen.
         */
        virtual int visibleLines() const;

        /**
         * Returns the total number of lines.
         *
         * Reimplement this with the number of lines the line browser will
         * scroll through.
         *
         * \return The total number of lines to browse through.
         */
        virtual int lineCount() const = 0;

        /**
         * Makes sure that the currently selected index is visible on the
         * screen.
         *
         * This should be called whenever _selectedIndex is changed to make sure
         * that the cursor is visible on the screen.
         */
        virtual void makeSelectionVisible();

        int _offset;
        int _selectedIndex;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

