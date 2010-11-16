/* ner: src/util.hh
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

#ifndef NER_UTIL_H
#define NER_UTIL_H 1

#include <string>
#include <time.h>
#include <gmime/gmime.h>

#include "gmime_iostream.hh"
#include "ncurses.hh"
#include "ner_config.hh"
#include "message_part.hh"

std::string relativeTime(time_t rawTime);

std::string formatByteSize(long size);

template <typename Type>
    struct addressOf : public std::unary_function<Type, Type *>
{
    const Type * operator()(const Type & x) const
    {
        return &x;
    }
};

template <class OutputIterator>
    void processMimePart(GMimeObject * part, OutputIterator destination)
{
    GMimeContentType * contentType = g_mime_object_get_content_type(part);

    std::string disposition = g_mime_object_get_disposition(part) ? : std::string();

    if (GMIME_IS_PART(part))
    {
        if (disposition == "attachment" || !g_mime_content_type_is_type(contentType, "text", "*"))
            *destination++ = std::make_shared<Attachment>(GMIME_PART(part));
        else
            *destination++ = std::make_shared<TextPart>(GMIME_PART(part));
    }
    else if (g_mime_content_type_is_type(contentType, "multipart", "alternative"))
    {
        std::map<int, std::pair<const char *, const char *>> contentTypePriorities{
            { 100,  std::make_pair("text", "plain") },
            { 50,   std::make_pair("text", "html") },
            { 20,   std::make_pair("text", "*") },
            { 1,    std::make_pair("*", "*") }
        };

        GMimeObject * bestPart = NULL;
        int bestPriority = 0;

        for (int index = 0, count = g_mime_multipart_get_count(GMIME_MULTIPART(part));
            index < count; ++index)
        {
            GMimeObject * currentPart = g_mime_multipart_get_part(GMIME_MULTIPART(part), index);
            GMimeContentType * partContentType = g_mime_object_get_content_type(currentPart);

            for (auto priority = contentTypePriorities.upper_bound(bestPriority),
                e = contentTypePriorities.end(); priority != e; ++priority)
            {
                if (g_mime_content_type_is_type(partContentType, priority->second.first,
                    priority->second.second))
                {
                    bestPart = currentPart;
                    bestPriority = priority->first;
                }
            }
        }

        processMimePart(bestPart, destination);
    }
    else if (g_mime_content_type_is_type(contentType, "multipart", "*"))
    {
        for (int index = 0, count = g_mime_multipart_get_count(GMIME_MULTIPART(part));
            index < count; ++index)
        {
            processMimePart(g_mime_multipart_get_part(GMIME_MULTIPART(part), index), destination);
        }
    }
}

template <typename Function>
    class OnScopeEnd
{
    Function _function;

public:
    OnScopeEnd(Function function)
        : _function(function)
    {}

    ~OnScopeEnd()
    {
        _function();
    }
};

template<typename Function>
    OnScopeEnd<Function> onScopeEnd(Function function)
{
    return OnScopeEnd<Function>(function);
}

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

