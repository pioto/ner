/* ner: src/colors.hh
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

#ifndef NER_COLORS_H
#define NER_COLORS_H 1

namespace Colors
{
    enum Color
    {
        /* General */
        CUT_OFF_INDICATOR = 1,

        /* Status Bar */
        STATUS_BAR_STATUS,
        STATUS_BAR_MESSAGE,
        STATUS_BAR_PROMPT,

        /* Search View */
        SEARCH_VIEW_DATE,
        SEARCH_VIEW_MESSAGE_COUNT_COMPLETE,
        SEARCH_VIEW_MESSAGE_COUNT_PARTIAL,
        SEARCH_VIEW_AUTHORS,
        SEARCH_VIEW_SUBJECT,
        SEARCH_VIEW_TAGS,

        /* Thread View */
        THREAD_VIEW_ARROW,

        /* Message View */
        MESSAGE_VIEW_HEADER
    };
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

