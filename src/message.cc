#include "message.hh"
#include "notmuch.hh"

InvalidMessageException::InvalidMessageException(const std::string & messageId)
    : _id(messageId)
{
}

InvalidMessageException::~InvalidMessageException() throw()
{
}

const char * InvalidMessageException::what() const throw()
{
    return ("Cannot find message with ID: " + _id).c_str();
}

Message::Message(notmuch_message_t * message)
{
    id       = notmuch_message_get_message_id(message);
    filename = notmuch_message_get_filename(message);
    date     = notmuch_message_get_date(message);


    headers = {
        { "From",    notmuch_message_get_header(message, "From")    ? : "(null)" },
        { "To",      notmuch_message_get_header(message, "To")      ? : "(null)" },
        { "Subject", notmuch_message_get_header(message, "Subject") ? : "(null)" },
    };


    notmuch_tags_t * tagIterator;
    for (tagIterator = notmuch_message_get_tags(message);
        notmuch_tags_valid(tagIterator);
        notmuch_tags_move_to_next(tagIterator))
    {
        tags.insert(notmuch_tags_get(tagIterator));
    }
    notmuch_tags_destroy(tagIterator);


    notmuch_messages_t * messages;
    for (messages = notmuch_message_get_replies(message);
        notmuch_messages_valid(messages);
        notmuch_messages_move_to_next(messages))
    {
        replies.push_back(Message(notmuch_messages_get(messages)));
    }
    notmuch_messages_destroy(messages);
}

void Message::removeTag(std::string tag)
{
    auto message = Notmuch::message(id);

    notmuch_message_remove_tag(message, tag.c_str());
    tags.erase(tag);

    notmuch_message_destroy(message);
}

void Message::removeTagRecursive(std::string tag)
{
    removeTag(tag);

    for (Message m : replies) {
        m.removeTagRecursive(tag);
    }
}

void Message::addTag(std::string tag)
{
    auto message = Notmuch::message(id);

    notmuch_message_add_tag(message, tag.c_str());
    tags.insert(tag);

    notmuch_message_destroy(message);
}

void Message::addTagRecursive(std::string tag)
{
    addTag(tag);

    for (Message m : replies) {
        m.addTagRecursive(tag);
    }
}
