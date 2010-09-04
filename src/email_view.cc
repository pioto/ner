/* ner: src/email_view.cc
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

#include "email_view.hh"
#include "colors.hh"
#include "ncurses.hh"
#include "util.hh"

const std::string lessMessage("[less]");
const std::string moreMessage("[more]");
const std::string whitespace(" \t\r\n");
const int messageWrapWidth = 80;

class WrappedLineIterator : public std::iterator<std::input_iterator_tag, std::string>
{
    public:
        explicit WrappedLineIterator(const std::string & line, int maxWidth = messageWrapWidth)
            : _line(line), _maxWidth(maxWidth), _position(0)
        {
            updateWidth();
        }

        /**
         * Default constructor used as an iterator indicating the end of the
         * line.
         */
        WrappedLineIterator(int maxWidth = messageWrapWidth)
            : _maxWidth(maxWidth), _position(std::string::npos)
        {
        }

        WrappedLineIterator & operator++()
        {
            if (_width == std::string::npos)
                _position = std::string::npos;
            else
            {
                _position = _line.find_first_not_of(whitespace, _position + _width);
                updateWidth();
            }

            return *this;
        }

        std::string operator*() const
        {
            return _line.substr(_position, _width);
        }

        bool operator==(const WrappedLineIterator & other) const
        {
            return ((_position == std::string::npos && other._position == std::string::npos) ||
                (_position == other._position && _maxWidth == other._maxWidth &&
                    _line == other._line));
        }

        bool operator!=(const WrappedLineIterator & other) const
        {
            return ((_position != std::string::npos || other._position != std::string::npos) &&
                (_position != other._position || _maxWidth != other._maxWidth ||
                    _line != other._line));
        }

        void updateWidth()
        {
            size_t lastWhitespaceIndex;

            if (_line.size() > _position + _maxWidth)
            {
                if (isspace(_line.at(_position + _maxWidth)))
                    lastWhitespaceIndex = _position + _maxWidth;
                else
                {
                    size_t index = _line.find_last_of(whitespace, _position + _maxWidth);

                    if (index == std::string::npos || index < _position)
                        lastWhitespaceIndex = _position + _maxWidth;
                    else
                        lastWhitespaceIndex = index;
                }

                _width = _line.find_last_not_of(whitespace, lastWhitespaceIndex) - _position + 1;
            }
            else
                _width = std::string::npos;
        }

        bool wrapped() const
        {
            return _position != 0;
        }

    private:
        size_t _position;
        size_t _width;
        int _maxWidth;
        std::string _line;
};

EmailView::EmailView(const View::Geometry & geometry)
    : LineBrowserView(geometry),
        _visibleHeaders{
            "From",
            "To",
            "Subject",
        }
{
    /* Colors */
    init_pair(Colors::EMAIL_VIEW_HEADER,  COLOR_CYAN, COLOR_BLACK);
}

EmailView::~EmailView()
{
}

void EmailView::setEmail(const std::string & filename)
{
    _lines.clear();

    FILE * file = fopen(filename.c_str(), "r");

    if (file != NULL)
    {
        GMimeStream * stream = g_mime_stream_file_new(file);
        GMimeParser * parser = g_mime_parser_new_with_stream(stream);
        GMimeMessage * message = g_mime_parser_construct_message(parser);

        /* Read relavant headers */
        _headers = {
            { "To",         internet_address_list_to_string(g_mime_message_get_recipients(message,
                GMIME_RECIPIENT_TYPE_TO), true) ? : "(null)" },
            { "From",       g_mime_message_get_sender(message) ? : "(null)" },
            { "Cc",         internet_address_list_to_string(g_mime_message_get_recipients(message,
                GMIME_RECIPIENT_TYPE_CC), true) ? : "(null)" },
            { "Bcc",         internet_address_list_to_string(g_mime_message_get_recipients(message,
                GMIME_RECIPIENT_TYPE_BCC), true) ? : "(null)" },
            { "Subject",    g_mime_message_get_subject(message) ? : "(null)" }
        };

        GMimeObject * mimePart = g_mime_message_get_mime_part(message);

        /* Locate plain text parts */
        mimePartLines(mimePart, std::back_inserter(_lines));

        g_object_unref(mimePart);

        calculateLines();
    }
}

void EmailView::setVisibleHeaders(const std::vector<std::string> & headers)
{
    _visibleHeaders = headers;
}

void EmailView::update()
{
    int row = 0;

    werase(_window);

    for (auto header = _visibleHeaders.begin(), e = _visibleHeaders.end(); header != e; ++header, ++row)
    {
        int x = 0;

        wmove(_window, row, x);

        try
        {
            x += NCurses::addPlainString(_window, (*header) + ": ",
                0, Colors::EMAIL_VIEW_HEADER);

            NCurses::checkMove(_window, x);

            x += NCurses::addUtf8String(_window, _headers[*header].c_str());

            NCurses::checkMove(_window, x - 1);
        }
        catch (const NCurses::CutOffException & e)
        {
            NCurses::addCutOffIndicator(_window);
        }
    }

    wmove(_window, row, 0);
    whline(_window, 0, getmaxx(_window));
    ++row;

    for (auto line = _lines.begin(), e = _lines.end(), messageRow = 0;
        line != e && row < getmaxy(_window); ++line)
    {
        for (auto wrappedLine = WrappedLineIterator(*line), e = WrappedLineIterator();
            wrappedLine != e && row < getmaxy(_window); ++wrappedLine, ++messageRow)
        {
            if (messageRow < _offset)
                continue;

            bool selected = _selectedIndex == messageRow;

            if (wrappedLine.wrapped())
                mvwaddch(_window, row, 0, ACS_CKBOARD | COLOR_PAIR(Colors::LINE_WRAP_INDICATOR));

            wmove(_window, row, 2);

            attr_t attributes = 0;

            if (selected)
            {
                attributes |= A_REVERSE;
                wchgat(_window, -1, A_REVERSE, 0, NULL);
            }

            if (NCurses::addUtf8String(_window, (*wrappedLine).c_str(), attributes) > getmaxx(_window))
                NCurses::addCutOffIndicator(_window, attributes);

            ++row;
        }
    }

    for (; row < getmaxy(_window); ++row)
        mvwaddch(_window, row, 0, '~' | A_BOLD | COLOR_PAIR(Colors::EMPTY_SPACE_INDICATOR));

    wattron(_window, COLOR_PAIR(Colors::MORE_LESS_INDICATOR));

    if (_offset > 0)
        mvwaddstr(_window, _visibleHeaders.size() + 1, getmaxx(_window) - lessMessage.size(), lessMessage.c_str());

    if (_offset + visibleLines() < lineCount())
        mvwaddstr(_window, getmaxy(_window) - 1, getmaxx(_window) - moreMessage.size(), moreMessage.c_str());

    wattroff(_window, COLOR_PAIR(Colors::MORE_LESS_INDICATOR));
}

void EmailView::calculateLines()
{
    _lineCount = 0;

    for (auto line = _lines.begin(), e = _lines.end();
        line != e; ++line)
    {
        for (auto wrappedLine = WrappedLineIterator(*line), e = WrappedLineIterator();
            wrappedLine != e; ++wrappedLine, ++_lineCount);
    }
}

int EmailView::visibleLines() const
{
    return getmaxy(_window) - _visibleHeaders.size() - 1;
}

int EmailView::lineCount() const
{
    return _lineCount;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

