/* ner: src/view_manager.hh
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

#ifndef NER_VIEW_MANAGER_H
#define NER_VIEW_MANAGER_H 1

#include <vector>

#include "input_handler.hh"

class View;

class ViewManager : public InputHandler
{
    public:
        ViewManager();
        ~ViewManager();

        virtual bool handleKeySequence(const std::vector<int> & sequence);

        void addView(View * view);
        void closeActiveView();
        void update();
        void refresh();

    private:
        View * _activeView;
        std::vector<View *> _views;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8


