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
#include <vector>

#include "thread.hh"

#include <notmuch.h>
#include <glib.h>

namespace Notmuch
{
    void initializeDatabase(const std::string & path);
    void closeDatabase();

    notmuch_database_t * openDatabase(notmuch_database_mode_t mode = NOTMUCH_DATABASE_MODE_READ_ONLY);

    unsigned countMessages(std::string query);
    std::vector<Thread> & searchThreads(std::string query);

    notmuch_thread_t * thread(std::string id, notmuch_query_t ** queryp);
    Thread & getThread(std::string id);

    notmuch_message_t * message(std::string id);
    Message & getMessage(std::string id);

    GKeyFile * config();
};

#endif
