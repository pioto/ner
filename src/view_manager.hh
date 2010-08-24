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
#include <memory>

#include "input_handler.hh"

class View;

/**
 * Manages the currently active Views.
 *
 * This class is a singleton.
 */
class ViewManager : public InputHandler
{
    public:
        static ViewManager & instance()
        {
            return *_instance;
        }

        ViewManager();
        ~ViewManager();

        /**
         * Handles the given input sequence.
         *
         * First, the ViewManager itself will attempt to handle the sequence,
         * and if it cannot, will try with the currently active view.
         *
         * \param sequence The sequence to handle.
         */
        virtual InputHandler::HandleResult handleKeySequence(const std::vector<int> & sequence);

        /**
         * Adds the view to the managed views, then sets it as the active view.
         *
         * \param view The view to manage
         */
        void addView(const std::shared_ptr<View> & view);

        /**
         * Closes the active view.
         *
         * The view will be deleted.
         */
        void closeActiveView();

        void update();
        void refresh();
        void resize();

        const View & activeView() const;

    private:
        static ViewManager * _instance;

        void openView(int index);
        void closeView(int index);

        std::shared_ptr<View> _activeView;
        std::vector<std::shared_ptr<View>> _views;

    friend class ViewView;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8


