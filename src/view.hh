/* ner: src/view.hh
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

#ifndef NER_VIEW_H
#define NER_VIEW_H 1

#include <stdint.h>
#include <ncursesw/ncurses.h>
#include <vector>
#include <string>

#include "input_handler.hh"
#include "status_bar.hh"

/**
 * The base class for all types of views
 */
class View : public InputHandler
{
    public:
        virtual ~View() = 0;

        /* Abstract methods */

        /**
         * Updates the screen.
         *
         * This gets called after each input sequence is successfully handled.
         */
        virtual void update() = 0;

        /**
         * Refreshes the screen.
         */
        virtual void refresh() = 0;

        /**
         * Resizes the screen to the given size and position.
         */
        virtual void resize(int x, int y, int width, int height) = 0;

        /**
         * \overload
         */
        virtual void resize();

        /**
         * Called when focus gets transfered to this view.
         */
        virtual void focus();

        /**
         * Called when focus gets transfered away from this view.
         */
        virtual void unfocus();

        virtual std::string name() const = 0;
        virtual std::vector<std::string> status() const;

    protected:
        static inline int defaultX() { return 0; }
        static inline int defaultY() { return 0; }
        static inline int defaultWidth() { return COLS; }
        static inline int defaultHeight() { return LINES - StatusBar::instance().height(); }

        virtual void updateStatus();

    friend class ViewManager;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

