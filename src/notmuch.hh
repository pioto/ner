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
#include <glib.h>

namespace NotMuch
{
    class InvalidThreadException : public std::exception
    {
        public:
            InvalidThreadException(const std::string & threadId);
            ~InvalidThreadException() throw();

            virtual const char * what() const throw();

        private:
            std::string _id;
    };

    class InvalidMessageException : public std::exception
    {
        public:
            InvalidMessageException(const std::string & messageId);
            ~InvalidMessageException() throw();

            virtual const char * what() const throw();

        private:
            std::string _id;
    };

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

    template <class T>
        class MessageTreeIterator
    {
        public:
            template <class InputIterator>
                explicit MessageTreeIterator(InputIterator begin, InputIterator end)
            {
                for (; begin < end; ++begin)
                    messages.push_back(&*begin);
            }

            MessageTreeIterator()
            {
            }

            typedef T value_type;
            typedef T & reference;
            typedef T * pointer;
            typedef std::ptrdiff_t difference_type;
            typedef std::input_iterator_tag iterator_category;

            reference operator*() const
            {
                return *messages.back();
            }

            pointer operator->() const
            {
                return messages.back();
            }

            reference operator++()
            {
                reference message = *messages.back();
                messages.pop_back();

                for (auto reply = message.replies.rbegin(), e = message.replies.rend();
                    reply != e; ++reply)
                {
                    messages.push_back(&*reply);
                }
            }

            bool operator==(const MessageTreeIterator & other) const
            {
                return messages.size() == other.messages.size() &&
                    (messages.size() == 0 || messages.back()->id == other.messages.back()->id);
            }

            bool operator!=(const MessageTreeIterator & other) const
            {
                return !operator==(other);
            }

        private:
            std::vector<T *> messages;
    };

    struct Message
    {
        typedef MessageTreeIterator<Message> iterator;
        typedef MessageTreeIterator<const Message> const_iterator;

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

    GKeyFile * config();
    void setConfig(const std::string & path);
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

