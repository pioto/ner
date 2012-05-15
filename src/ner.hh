/* ner: src/ner.hh
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

#ifndef NER_NER_H
#define NER_NER_H 1

#include <string>
#include <vector>

#include "input_handler.hh"
#include "view_manager.hh"
#include "status_bar.hh"

class Ner : public InputHandler
{
    public:
        Ner();
        ~Ner();

        void run();
        void quit();

        void search();
        void compose();
        void openMessage();
        void openThread();
        void openViewView();
        void redraw();

        inline ViewManager & viewManager()
        {
            return _viewManager;
        }

    private:
        bool _running;
        ViewManager _viewManager;
        StatusBar _statusBar;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

