/* ner: src/util.hh
 *
 * Copyright (c) 2010 Michael Forney
 *
 * This file is a part of ner.
 *
 * ner is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2, as published by the Free
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

#ifndef NER_UTIL_H
#define NER_UTIL_H 1

#include <string>
#include <time.h>

enum
{
    NER_COLOR_RED = 1,
    NER_COLOR_GREEN,
    NER_COLOR_YELLOW,
    NER_COLOR_BLUE,
    NER_COLOR_CYAN,
    NER_COLOR_MAGENTA,
    NER_COLOR_WHITE
};

std::string relativeTime(time_t rawTime);

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

