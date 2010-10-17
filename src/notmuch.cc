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

using namespace NotMuch;

GKeyFile * _config = NULL;

NotMuch::InvalidThreadException::InvalidThreadException(const std::string & threadId)
    : _id(threadId)
{
}

NotMuch::InvalidThreadException::~InvalidThreadException() throw()
{
}

const char * NotMuch::InvalidThreadException::what() const throw()
{
    return ("Cannot find thread with ID: " + _id).c_str();
}

NotMuch::InvalidMessageException::InvalidMessageException(const std::string & messageId)
    : _id(messageId)
{
}

NotMuch::InvalidMessageException::~InvalidMessageException() throw()
{
}

const char * NotMuch::InvalidMessageException::what() const throw()
{
    return ("Cannot find message with ID: " + _id).c_str();
}

Thread::Thread(notmuch_thread_t * thread)
    : id(notmuch_thread_get_thread_id(thread)),
        subject(notmuch_thread_get_subject(thread) ? : "(null)"),
        authors(notmuch_thread_get_authors(thread) ? : "(null)"),
        totalMessages(notmuch_thread_get_total_messages(thread)),
        matchedMessages(notmuch_thread_get_matched_messages(thread)),
        newestDate(notmuch_thread_get_newest_date(thread)),
        oldestDate(notmuch_thread_get_oldest_date(thread))
{
    notmuch_tags_t * tagIterator;

    for (tagIterator = notmuch_thread_get_tags(thread);
        notmuch_tags_valid(tagIterator);
        notmuch_tags_move_to_next(tagIterator))
    {
        tags.insert(notmuch_tags_get(tagIterator));
    }

    notmuch_tags_destroy(tagIterator);
}

Message::Message(notmuch_message_t * message)
    : id(notmuch_message_get_message_id(message)),
        filename(notmuch_message_get_filename(message)),
        date(notmuch_message_get_date(message)),
        matched(notmuch_message_get_flag(message, NOTMUCH_MESSAGE_FLAG_MATCH)),
        headers{
            {"From",    notmuch_message_get_header(message, "From")     ? : "(null)"},
            {"To",      notmuch_message_get_header(message, "To")       ? : "(null)"},
            {"Subject", notmuch_message_get_header(message, "Subject")  ? : "(null)"},
        }
{
    /* Tags */
    notmuch_tags_t * tagIterator;

    for (tagIterator = notmuch_message_get_tags(message);
        notmuch_tags_valid(tagIterator);
        notmuch_tags_move_to_next(tagIterator))
    {
        tags.insert(notmuch_tags_get(tagIterator));
    }

    notmuch_tags_destroy(tagIterator);

    /* Replies */
    notmuch_messages_t * messages;

    for (messages = notmuch_message_get_replies(message);
        notmuch_messages_valid(messages);
        notmuch_messages_move_to_next(messages))
    {
        replies.push_back(Message(notmuch_messages_get(messages)));
    }

    notmuch_messages_destroy(messages);
}

void Message::addTag(const std::string & tag)
{
    notmuch_database_t * database = openDatabase(NOTMUCH_DATABASE_MODE_READ_WRITE);
    notmuch_message_t * message = notmuch_database_find_message(database, id.c_str());

    auto status = notmuch_message_add_tag(message, tag.c_str());

    notmuch_database_close(database);

    switch (status)
    {
        case NOTMUCH_STATUS_SUCCESS:
            tags.insert(tag);
            return;
        case NOTMUCH_STATUS_TAG_TOO_LONG:
            throw std::runtime_error("Tag too long");
        default:
            throw std::exception();
    }
}

void Message::removeTag(const std::string & tag)
{
    notmuch_database_t * database = openDatabase(NOTMUCH_DATABASE_MODE_READ_WRITE);
    notmuch_message_t * message = notmuch_database_find_message(database, id.c_str());

    auto status = notmuch_message_remove_tag(message, tag.c_str());

    notmuch_database_close(database);

    switch (status)
    {
        case NOTMUCH_STATUS_SUCCESS:
            tags.erase(tag);
            return;
        case NOTMUCH_STATUS_TAG_TOO_LONG:
            throw std::runtime_error("Tag too long");
        default:
            throw std::exception();
    }
}

notmuch_database_t * NotMuch::openDatabase(notmuch_database_mode_t mode)
{
    return notmuch_database_open(g_key_file_get_string(_config, "database", "path", NULL), mode);
}

GKeyFile * NotMuch::config()
{
    return _config;
}

void NotMuch::setConfig(const std::string & path)
{
    if (_config)
        g_object_unref(_config);

    _config = g_key_file_new();

    if (!g_key_file_load_from_file(_config, path.c_str(), G_KEY_FILE_NONE, NULL))
        _config = NULL;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

