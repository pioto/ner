#ifndef NER_MESSAGE_H
#define NER_MESSAGE_H 1

#include <string>
#include <vector>
#include <map>
#include <set>

#include "notmuch.h"


class InvalidMessageException : public std::exception
{
    public:
        InvalidMessageException(const std::string & messageId);
        ~InvalidMessageException() throw();

        virtual const char * what() const throw();

    private:
        std::string _id;
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


class Message
{
    public:
        typedef MessageTreeIterator<Message> iterator;
        typedef MessageTreeIterator<const Message> const_iterator;

        Message(notmuch_message_t * message);

        void addTag(std::string tag);
        void addTagRecursive(std::string tag);

        void removeTag(std::string tag);
        void removeTagRecursive(std::string tag);

        std::string id;
        std::string filename;

        time_t date;

        std::map<std::string, std::string> headers;
        std::set<std::string> tags;
        std::vector<Message> replies;
};

#endif /* NER_MESSAGE_H */
