/* ner: src/notmuch.hh
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

#ifndef NER_NOTMUCH_H
#define NER_NOTMUCH_H 1

#include <string>
#include <set>
#include <map>
#include <vector>
#include <notmuch.h>

namespace NotMuch
{
    struct Thread
    {
        Thread(notmuch_thread_t * thread);

        std::string id;
        std::string subject;
        std::string authors;
        uint32_t totalMessages;
        uint32_t matchedMessages;
        time_t newestDate;
        time_t oldestDate;
        std::set<std::string> tags;
    };

    struct Message
    {
        Message() = default;
        Message(notmuch_message_t * message);

        std::string id;
        std::string filename;
        time_t date;
        bool matched;
        std::map<std::string, std::string> headers;
        std::set<std::string> tags;
        std::vector<Message> replies;
    };

    notmuch_database_t * openDatabase(notmuch_database_mode_t mode = NOTMUCH_DATABASE_MODE_READ_ONLY);

    const std::string & databasePath();
    void setDatabasePath(const std::string & path);
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

