/* ner: src/view_view.hh
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

#ifndef NER_VIEW_VIEW_H
#define NER_VIEW_VIEW_H 1

#include "line_browser_view.hh"

class ViewView : public LineBrowserView
{
    public:
        ViewView(int x = defaultX(), int y = defaultY(),
            int width = defaultWidth(), int height = defaultHeight());
        virtual ~ViewView();

        virtual void update();
        virtual void unfocus();

        virtual std::string name() const { return "view-view"; }
        virtual Type type() const { return Type::ViewView; }

        virtual int lineCount() const;

        void updateViews();
        void openSelectedView();

    protected:
        struct ViewInfo
        {
            std::string name;
        };

        std::vector<ViewInfo> _views;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

