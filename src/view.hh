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

class ViewManager;

class View : public InputHandler
{
    public:
        virtual ~View();

        /* Abstract methods */
        virtual void update() = 0;
        virtual void refresh() = 0;
        virtual void resize(int x = defaultX(), int y = defaultY(),
            int width = defaultWidth(), int height = defaultHeight()) = 0;
        virtual void focus();

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

