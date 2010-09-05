/* ner: src/util.hh
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

#ifndef NER_UTIL_H
#define NER_UTIL_H 1

#include <string>
#include <time.h>
#include <gmime/gmime.h>

#include "gmime_iostream.hh"
#include "ncurses.hh"

std::string relativeTime(time_t rawTime);

template <typename Type>
    struct addressOf : public std::unary_function<Type, Type *>
{
    const Type * operator()(const Type & x) const
    {
        return &x;
    }
};

template <class OutputIterator>
    void mimePartLines(GMimeObject * part, OutputIterator destination)
{
    GMimeContentType * contentType = g_mime_object_get_content_type(part);

    /* If this part is html */
    if (g_mime_content_type_is_type(contentType, "text", "html"))
    {
        /* TODO: Handle html email */
    }
    /* If this part is plain text */
    if (g_mime_content_type_is_type(contentType, "text", "*"))
    {
        GMimeDataWrapper * content = g_mime_part_get_content_object(GMIME_PART(part));
        const char * charset = g_mime_object_get_content_type_parameter(part, "charset");
        GMimeStream * contentStream = g_mime_data_wrapper_get_stream(content);
        GMimeStream * filteredStream = g_mime_stream_filter_new(contentStream);

        GMimeFilter * filter = g_mime_filter_basic_new(g_mime_data_wrapper_get_encoding(content), false);
        g_mime_stream_filter_add(GMIME_STREAM_FILTER(filteredStream), filter);
        g_object_unref(filter);

        if (charset)
        {
            GMimeFilter * filter = g_mime_filter_charset_new(charset, "UTF-8");
            g_mime_stream_filter_add(GMIME_STREAM_FILTER(filteredStream), filter);
            g_object_unref(filter);
        }

        g_mime_stream_reset(contentStream);

        GMimeIOStream stream(filteredStream);
        g_object_unref(filteredStream);

        while (stream.good())
        {
            std::string line;
            std::getline(stream, line);
            for (std::size_t tab = 0; (tab = line.find('\t', tab)) != std::string::npos; ++tab)
                line.replace(tab, 1, 8 - (tab % 8), ' ');
            *destination++ = line;
        }
    }
    else if (g_mime_content_type_is_type(contentType, "multipart", "*"))
    {
        for (int index = 0, count = g_mime_multipart_get_count(GMIME_MULTIPART(part));
            index < count;
            ++index)
        {
            mimePartLines(g_mime_multipart_get_part(GMIME_MULTIPART(part), index), destination);
        }
    }
}

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

