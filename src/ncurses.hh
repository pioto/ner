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

#include <limits>
#include <algorithm>
#include <locale>
#include <cstring>
#include <ncursesw/ncurses.h>

#include "colors.hh"

namespace NCurses
{
    class CutOffException : public std::exception
    {
        public:
            ~CutOffException() throw ();

            const char * what() throw ();
    };

    inline void checkMove(WINDOW * window, int x)
    {
        if (wmove(window, getcury(window), x) == ERR)
            throw NCurses::CutOffException();
    }

    inline void addCutOffIndicator(WINDOW * window, attr_t attributes = 0)
    {
        wmove(window, getcury(window), getmaxx(window) - 1);
        waddch(window, '$' | attributes | COLOR_PAIR(Colors::CUT_OFF_INDICATOR));
    }

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

    inline int addPlainString(WINDOW * window, const std::string & string,
        attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max())
    {
        return addPlainString(window, string.begin(), string.end(), attributes, color, maxLength);
    }

    inline int addPlainString(WINDOW * window, const char * string,
        attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max())
    {
        return addPlainString(window, string, string + std::strlen(string), attributes, color, maxLength);
    }

    inline int addChar(WINDOW * window, chtype character,
        int attributes = 0, short color = 0)
    {
        character |= attributes | COLOR_PAIR(color);
        waddchnstr(window, &character, 1);
        return 1;
    }

    inline int addUtf8String(WINDOW * window, const char * string,
        attr_t attributes = 0, short color = 0, int maxLength = std::numeric_limits<int>::max())
    {
        mbstate_t state = { 0 };

        int length = strlen(string);

        cchar_t displayCharacters[length + 1];
        int displayIndex = 0;

        wchar_t wideCharacters[CCHARW_MAX + 1];
        wchar_t wideCharacter;
        int wideIndex = 0;

        for (int position = 0; position < length;)
        {
            int bytesRead = std::mbrtowc(&wideCharacter,
                string + position, length - position, &state);

            position += bytesRead;

            if (bytesRead < 0)
                break;

            int width = wcwidth(wideCharacter);

            /* We found a new spacing character, set the next cchar_t */
            if ((width > 0 && wideIndex > 0) || wideIndex == CCHARW_MAX)
            {
                wideCharacters[wideIndex] = L'\0';
                setcchar(&displayCharacters[displayIndex++], wideCharacters,
                    attributes, color, NULL);

                /* Start the next display character */
                wideIndex = 0;
            }
            else if (width == 0 && wideIndex == 0)
                wideCharacters[wideIndex++] = L' ';
            else if (width < 0)
                break;

            wideCharacters[wideIndex++] = wideCharacter;
        }

        if (wideIndex > 0)
        {
            wideCharacters[wideIndex] = L'\0';
            setcchar(&displayCharacters[displayIndex++], wideCharacters,
                attributes, color, NULL);
        }

        /* Set the NULL cchar_t */
        wideCharacters[0] = L'\0';
        setcchar(&displayCharacters[displayIndex], wideCharacters, 0, 0, NULL);

        int displayLength = std::min(maxLength, displayIndex);

        wadd_wchnstr(window, displayCharacters, displayLength);

        return displayLength;
    }
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

