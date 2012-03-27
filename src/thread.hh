#ifndef NER_THREAD_H
#define NER_THREAD_H 1

#include <string>
#include <set>

#include "message.hh"

#include "notmuch.h"

class InvalidThreadException : public std::exception
{
    public:
        InvalidThreadException(const std::string & threadId);
        ~InvalidThreadException() throw();

        virtual const char * what() const throw();

    private:
        std::string _id;
};

class Thread
{
    public:
        Thread(notmuch_thread_t * thread);


        void topLevelMessages(std::vector<Message> & messages);

        void addTag(std::string tag);
        void removeTag(std::string tag);


        std::string id;
        std::string subject;
        std::string authors;

        uint32_t totalMessages;
        uint32_t matchedMessages;

        time_t newestDate;
        time_t oldestDate;

        std::set<std::string> tags;
};

#endif /* NER_THREAD_H */
