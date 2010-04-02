/* ner: src/window_view.hh
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

#ifndef NER_WINDOW_VIEW_H
#define NER_WINDOW_VIEW_H 1

#include "view.hh"

class WindowView : public View
{
    public:
        WindowView(int x, int y, int width, int height);
        virtual ~WindowView();

        virtual void refresh();
        virtual void resize(int width, int height, int x, int y);

    protected:
        WINDOW * _window;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8


