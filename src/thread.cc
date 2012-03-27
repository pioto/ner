#include "thread.hh"
#include "notmuch.hh"

InvalidThreadException::InvalidThreadException(const std::string & threadId)
    : _id(threadId)
{
}

InvalidThreadException::~InvalidThreadException() throw()
{
}

const char * InvalidThreadException::what() const throw()
{
    return ("Cannot find thread with ID: " + _id).c_str();
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

void Thread::topLevelMessages(std::vector<Message> & messages)
{
    notmuch_query_t * query = NULL;
    notmuch_thread_t * thread = Notmuch::thread(id, &query);

    messages.clear();

    notmuch_messages_t * x;
    for (x = notmuch_thread_get_toplevel_messages(thread);
        notmuch_messages_valid(x);
        notmuch_messages_move_to_next(x))
    {
        messages.push_back(*new Message(notmuch_messages_get(x)));
    }

    notmuch_query_destroy(query);
}

void Thread::addTag(std::string tag)
{
    tags.insert(tag);

    std::vector<Message> messages;
    topLevelMessages(messages);

    for (Message m : messages) {
        m.addTagRecursive(tag);
    }
}

void Thread::removeTag(std::string tag)
{
    tags.erase(tag);

    std::vector<Message> messages;
    topLevelMessages(messages);

    for (Message m : messages) {
        m.removeTagRecursive(tag);
    }
}
