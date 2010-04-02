/* ner: src/thread_message_view.cc
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

#include "thread_message_view.hh"
#include "notmuch.hh"
#include "colors.hh"

const int threadViewHeight = 8;

ThreadMessageView::ThreadMessageView(notmuch_thread_t * thread, int x, int y, int width, int height)
    : _threadView(thread, x, y, width, threadViewHeight),
        _messageView(x, y + threadViewHeight + 1, width, height - threadViewHeight - 1)
{
    /* Key Sequences */
    addHandledSequence("j",         std::bind(&MessageView::next, &_messageView));
    addHandledSequence(KEY_DOWN,    std::bind(&MessageView::next, &_messageView));
    addHandledSequence("k",         std::bind(&MessageView::previous, &_messageView));
    addHandledSequence(KEY_UP,      std::bind(&MessageView::previous, &_messageView));

    addHandledSequence(KEY_NPAGE,   std::bind(&MessageView::nextPage, &_messageView));
    addHandledSequence('d' - 96,    std::bind(&MessageView::nextPage, &_messageView)); // Ctrl-D
    addHandledSequence(KEY_PPAGE,   std::bind(&MessageView::previousPage, &_messageView));
    addHandledSequence('u' - 96,    std::bind(&MessageView::previousPage, &_messageView)); // Ctrl-U

    addHandledSequence("gg",        std::bind(&MessageView::moveToTop, &_messageView));
    addHandledSequence(KEY_HOME,    std::bind(&MessageView::moveToTop, &_messageView));
    addHandledSequence("G",         std::bind(&MessageView::moveToBottom, &_messageView));
    addHandledSequence(KEY_END,     std::bind(&MessageView::moveToBottom, &_messageView));

    addHandledSequence(" ",         std::bind(&ThreadMessageView::nextMessage, this));
    addHandledSequence('n' - 96,    std::bind(&ThreadMessageView::nextMessage, this));
    addHandledSequence('p' - 96,    std::bind(&ThreadMessageView::previousMessage, this));
}

ThreadMessageView::~ThreadMessageView()
{
}

ThreadMessageView * ThreadMessageView::fromId(const std::string & threadId)
{
    ThreadMessageView * threadMessageView;

    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_query_t * query = notmuch_query_create(database, ("thread:" + threadId).c_str());
    notmuch_threads_t * threads = notmuch_query_search_threads(query);

    if (notmuch_threads_valid(threads))
    {
        threadMessageView = new ThreadMessageView(notmuch_threads_get(threads));

        const std::string & messageId = threadMessageView->_threadView.selectedMessage().id;
        notmuch_message_t * message = notmuch_database_find_message(database, messageId.c_str());

        if (message)
            threadMessageView->_messageView.setMessage(message);
        else
            StatusBar::instance().displayMessage("Cannot find message with ID: " + messageId);

        notmuch_message_destroy(message);
    }
    else
    {
        threadMessageView = 0;
        StatusBar::instance().displayMessage("Cannot find thread with ID: " + threadId);
    }

    notmuch_threads_destroy(threads);
    notmuch_query_destroy(query);

    notmuch_database_close(database);

    return threadMessageView;
}

void ThreadMessageView::update()
{
    mvhline(threadViewHeight, 0, 0, COLS);

    _threadView.update();
    _messageView.update();
}

void ThreadMessageView::refresh()
{
    _threadView.refresh();
    _messageView.refresh();
}

void ThreadMessageView::resize(int x, int y, int width, int height)
{
    _threadView.resize(x, y, width, threadViewHeight);
    _messageView.resize(x, threadViewHeight + 1, width, height - threadViewHeight - 1);
}

void ThreadMessageView::nextMessage()
{
    _threadView.next();
    loadSelectedMessage();
    _messageView.moveToTop();
}

void ThreadMessageView::previousMessage()
{
    _threadView.previous();
    loadSelectedMessage();
    _messageView.moveToBottom();
}

void ThreadMessageView::loadSelectedMessage()
{
    const std::string & messageId = _threadView.selectedMessage().id;

    notmuch_database_t * database = NotMuch::openDatabase();
    notmuch_message_t * message = notmuch_database_find_message(database, messageId.c_str());

    if (message != NULL)
        _messageView.setMessage(message);
    else
        StatusBar::instance().displayMessage("Cannot find message with ID: " + messageId);

    notmuch_database_close(database);
}

std::vector<std::string> ThreadMessageView::status() const
{
    std::vector<std::string> mergedStatus(_threadView.status());
    const std::vector<std::string> & messageViewStatus(_messageView.status());
    std::copy(messageViewStatus.begin(), messageViewStatus.end(), std::back_inserter(mergedStatus));

    return mergedStatus;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

