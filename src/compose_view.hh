/* ner: src/compose_view.hh
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

#ifndef NER_COMPOSE_VIEW_H
#define NER_COMPOSE_VIEW_H 1

#include "email_edit_view.hh"

class ComposeView : public EmailEditView
{
    public:
        ComposeView(const View::Geometry & geometry = View::Geometry());
        virtual ~ComposeView();

        virtual std::string name() const { return "compose-view"; }

    protected:
        std::string _to;
        std::string _cc;
        std::string _subject;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

