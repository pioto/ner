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
    void processMimePart(GMimeObject * part, OutputIterator destination,
                         bool onlyFirstForAlternative = false)
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
        static std::vector<std::tuple<int, const char *, const char *>> contentTypePriorities{
            std::make_tuple( 100,  "text", "plain" ),
            std::make_tuple( 50,   "text", "html" ),
            std::make_tuple( 20,   "text", "*" ),
            std::make_tuple( 1,    "*", "*" )
        };

        int count = g_mime_multipart_get_count(GMIME_MULTIPART(part));
        std::vector<std::pair<GMimeObject*, int>> subpartsWithPriority;
        subpartsWithPriority.reserve(count);
        for (int index = 0; index < count; ++index)
        {
            GMimeObject * subpart = g_mime_multipart_get_part(GMIME_MULTIPART(part), index);
            GMimeContentType * subpartContentType = g_mime_object_get_content_type(subpart);
            int subpartPriority = 0;

            for (auto priority = contentTypePriorities.begin();
                 priority != contentTypePriorities.end(); ++priority)
            {
                if (subpartPriority < std::get<0>(*priority)
                    and g_mime_content_type_is_type(subpartContentType, std::get<1>(*priority), std::get<2>(*priority)))
                    subpartPriority = std::get<0>(*priority);
            }

            subpartsWithPriority.push_back(std::make_pair(subpart, subpartPriority));
        }
        std::sort(subpartsWithPriority.begin(), subpartsWithPriority.end(),
                  [] (const std::pair<GMimeObject*, int>& lhs,
                      const std::pair<GMimeObject*, int>& rhs)
                  { return lhs.second > rhs.second; });

        for (auto subpart = subpartsWithPriority.begin();
             subpart != subpartsWithPriority.end(); ++subpart)
        {
            processMimePart(subpart->first, destination);
            if (onlyFirstForAlternative)
                break;
        }
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

template<typename T>
class AutoUnref
{
    T* _g_object;

public:
    AutoUnref(T* g_object) : _g_object(g_object) {}
    ~AutoUnref() { if (_g_object) g_object_unref(_g_object); }

    operator T*() { return _g_object; }
};

template<typename T>
AutoUnref<T> autoUnref(T* g_object)
{
    return AutoUnref<T>(g_object);
}

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

