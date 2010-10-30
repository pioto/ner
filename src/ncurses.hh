/* ner: src/ncurses.hh
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

#ifndef NER_NCURSES_H
#define NER_NCURSES_H 1

#include "config.h"

#include <limits>
#include <algorithm>
#include <locale>
#include <cstring>

#if HAVE_NCURSESW_NCURSES_H
#   include <ncursesw/ncurses.h>
#elif HAVE_NCURSES_NCURSES_H
#   include <ncurses/ncurses.h>
#elif HAVE_NCURSES_H
#   include <ncurses.h>
#else
#   include <curses.h>
#endif

#include "colors.hh"

/**
 * NCurses utility functions
 */
namespace NCurses
{
    /**
     * An exception indicating that a move was attempted past the edge of the
     * screen.
     */
    class CutOffException : public std::exception
    {
        public:
            ~CutOffException() throw ();

            const char * what() throw ();
    };

    /**
     * Perform a checked move to the specified column.
     *
     * Throws a CutOffException if this coordinate is past the edge of the
     * screen.
     *
     * \param window The window in which to move.
     * \param x The column to move to.
     */
    void checkMove(WINDOW * window, int x);

    /**
     * Adds a cut-off indicator to the end of the current line.
     *
     * \param window The window in which to add the indicator.
     * \param attributes The attributes for the indicator.
     */
    void addCutOffIndicator(WINDOW * window, attr_t attributes = 0);

    /**
     * Adds a plain string to the window.
     *
     * The cursor is not advanced.
     *
     * \param window The window in which to add the string
     * \param first An input iterator pointing to the first character.
     * \param last An input iterator pointing to the end.
     * \param attributes The attributes of the string.
     * \param color The color of the string.
     * \param maxLength The maximum number of columns to print.
     */
    template <class InputIterator>
        int addPlainString(WINDOW * window, InputIterator first, InputIterator last,
            attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max())
    {
        int distance = std::distance(first, last);
        chtype characters[distance];

        std::transform(first, last, characters,
            std::bind1st(std::bit_or<chtype>(), attributes | COLOR_PAIR(color)));

        int length = std::min(distance, maxLength);

        waddchnstr(window, characters, length);

        return length;
    }

    /**
     * \overload
     */
    int addPlainString(WINDOW * window, const std::string & string,
        attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max());

    /**
     * \overload
     */
    int addPlainString(WINDOW * window, const char * string,
        attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max());

    /**
     * Adds a UTF-8 string to the window.
     *
     * The characters in string are expanded with mbrtowc, and then added to
     * the screen. The cursor is not advanced.
     *
     * \param window The window in which to print the string.
     * \param string The UTF-8 string to print.
     * \param attributes The attributes of the string.
     * \param color The color of the string.
     * \param maxLength The maximum number of columns the string should take up.
     */
    int addUtf8String(WINDOW * window, const char * string,
        attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max());

    /**
     * Adds a single character to the window.
     *
     * The cursor is not advanced.
     *
     * \param window The window in which to add the character.
     * \param character The character to add.
     * \param attributes The attributes of the character.
     * \param color The color of the character.
     */
    int addChar(WINDOW * window, chtype character,
        int attributes = 0, short color = 0);
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

