/* ner: src/notmuch.cc
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

#include <stdexcept>
#include <glib-object.h>

#include "notmuch.hh"

GKeyFile * _config = NULL;
notmuch_database_t * _notmuchDatabase = NULL;

notmuch_database_t * Notmuch::openDatabase(notmuch_database_mode_t mode)
{
    return _notmuchDatabase;
}

GKeyFile * Notmuch::config()
{
    return _config;
}

void Notmuch::initializeDatabase(const std::string & path)
{
    _config = g_key_file_new();
    if (!g_key_file_load_from_file(_config, path.c_str(), G_KEY_FILE_NONE, NULL))
        throw new std::string("Couldn't load config file");

    char * db = g_key_file_get_string(_config, "database", "path", NULL);
    notmuch_status_t s = notmuch_database_open(db, NOTMUCH_DATABASE_MODE_READ_WRITE, &_notmuchDatabase);
    if (s != NOTMUCH_STATUS_SUCCESS) {
        throw std::runtime_error("Open database failed: "+std::string(notmuch_status_to_string(s)));
    }
}

notmuch_database_t * Notmuch::readonlyDatabase()
{
    char * db = g_key_file_get_string(_config, "database", "path", NULL);

    notmuch_database_t * ret;
    notmuch_status_t s = notmuch_database_open(db, NOTMUCH_DATABASE_MODE_READ_ONLY, &ret);
    if (s != NOTMUCH_STATUS_SUCCESS) {
        throw std::runtime_error("Open database failed: "+std::string(notmuch_status_to_string(s)));
    }

    return ret;
}

void Notmuch::closeDatabase()
{
    notmuch_database_close(_notmuchDatabase);
}


unsigned Notmuch::countMessages(std::string query)
{
    unsigned ret;

    notmuch_query_t * x = notmuch_query_create(_notmuchDatabase,query.c_str());
    ret = notmuch_query_count_messages(x);
    notmuch_query_destroy(x);

    return ret;
}

std::vector<Thread> & Notmuch::searchThreads(std::string query)
{
}

notmuch_thread_t * Notmuch::thread(std::string id, notmuch_query_t ** queryp)
{
    const char * queryString = ("thread:" + id).c_str();
    notmuch_query_t * query = notmuch_query_create(_notmuchDatabase, queryString);
    notmuch_threads_t * threads = notmuch_query_search_threads(query);

    notmuch_thread_t * thread = NULL;
    if (notmuch_threads_valid(threads) && !!(thread = notmuch_threads_get(threads))) {
        *queryp = query;
        return thread;
    }

    notmuch_query_destroy(query);
    throw InvalidThreadException(id);
}

Thread & Notmuch::getThread(std::string id)
{
    notmuch_query_t * query = NULL;
    notmuch_thread_t * thread = Notmuch::thread(id, &query);

    Thread & ret = *(new Thread(thread));
    notmuch_query_destroy(query);

    return ret;
}

notmuch_message_t * Notmuch::message(std::string id)
{
    notmuch_message_t * message = NULL;
    notmuch_database_find_message(_notmuchDatabase, id.c_str(), &message);

    if (message == NULL)
        throw InvalidMessageException(id);

    return message;
}

Message & Notmuch::getMessage(std::string id)
{
    return *(new Message(Notmuch::message(id)));
}
