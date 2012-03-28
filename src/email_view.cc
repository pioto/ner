/* ner: src/email_view.cc
 *
 * Copyright (c) 2010 Michael Forney
 * Copyright (c) 2011 Maxime Coste
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
#include "status_bar.hh"
#include "message_part_display_visitor.hh"
#include "message_part_save_visitor.hh"

const std::string lessMessage("[less]");
const std::string moreMessage("[more]");

EmailView::EmailView(const View::Geometry & geometry)
    : LineBrowserView(geometry),
        _visibleHeaders{
            "From",
            "To",
            "Cc",
            "Subject",
        },
        _lineCount(0)
{
}

EmailView::~EmailView()
{
}

void EmailView::setEmail(const std::string & filename)
{
    _parts.clear();

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
        processMimePart(mimePart, std::back_inserter(_parts));
        if (not _parts.empty())
            _parts[0]->folded = false;

        g_object_unref(mimePart);
    }
}

void EmailView::setVisibleHeaders(const std::vector<std::string> & headers)
{
    _visibleHeaders = headers;
}

void EmailView::update()
{
    int row = 0;

    _partsEndLine.clear();
    werase(_window);

    for (auto header = _visibleHeaders.begin(), e = _visibleHeaders.end(); header != e; ++header, ++row)
    {
        int x = 0;

        wmove(_window, row, x);

        try
        {
            x += NCurses::addPlainString(_window, (*header) + ": ",
                0, ColorID::EmailViewHeader);

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
    whline(_window, 0, _geometry.width);
    ++row;

    MessagePartDisplayVisitor displayVisitor(_window, View::Geometry{ 0, row,
        _geometry.width, visibleLines() }, _offset, _selectedIndex, _parts.size() > 1);


    for (auto part = _parts.begin(), e = _parts.end(); part != e; ++part)
    {
        (*part)->accept(displayVisitor);
        _partsEndLine.push_back(displayVisitor.lines());
    }

    row = displayVisitor.row();
    _lineCount = displayVisitor.lines();

    for (; row < getmaxy(_window); ++row)
        mvwaddch(_window, row, 0, '~' | A_BOLD | COLOR_PAIR(ColorID::EmptySpaceIndicator));

    wattron(_window, COLOR_PAIR(ColorID::MoreLessIndicator));

    if (_offset > 0)
        mvwaddstr(_window, _visibleHeaders.size() + 1, _geometry.width - lessMessage.size(), lessMessage.c_str());

    if (_offset + visibleLines() < _lineCount)
        mvwaddstr(_window, getmaxy(_window) - 1, _geometry.width - moreMessage.size(), moreMessage.c_str());

    wattroff(_window, COLOR_PAIR(ColorID::MoreLessIndicator));
}

EmailView::PartList::iterator EmailView::selectedPart()
{
    for (size_t index = 0; index < _partsEndLine.size(); ++index)
    {
        if (_selectedIndex < _partsEndLine[index])
            return _parts.begin() + index;
    }
    return _parts.begin();
}

void EmailView::saveSelectedPart()
{
    MessagePartSaveVisitor saver;
    (*selectedPart())->accept(saver);
}

void EmailView::toggleSelectedPartFolding()
{
    PartList::iterator part = selectedPart();
    if (_parts.size() == 1)
        return;

    (*part)->folded = not (*part)->folded;

    if (part != _parts.begin())
        _selectedIndex = _partsEndLine[std::distance(_parts.begin(), part) - 1];
    else
        _selectedIndex = 0;

    makeSelectionVisible();

    StatusBar::instance().update();
    StatusBar::instance().refresh();
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

