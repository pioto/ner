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

ThreadMessageView::ThreadMessageView(const std::string & threadId, const View::Geometry & geometry)
    : _threadView(threadId, { geometry.x, geometry.y, geometry.width, threadViewHeight }),
        _messageView({
            geometry.x, geometry.y + threadViewHeight + 1,
            geometry.width, geometry.height - threadViewHeight - 1
        })
{
    _messageView.setMessage(_threadView.selectedMessage().id);

    /* Key Sequences */
    addHandledSequence("j",          std::bind(&MessageView::next, &_messageView));
    addHandledSequence("<Down>",     std::bind(&MessageView::next, &_messageView));
    addHandledSequence("k",          std::bind(&MessageView::previous, &_messageView));
    addHandledSequence("<Up>",       std::bind(&MessageView::previous, &_messageView));

    addHandledSequence("<PageDown>", std::bind(&MessageView::nextPage, &_messageView));
    addHandledSequence("<C-d>",      std::bind(&MessageView::nextPage, &_messageView));
    addHandledSequence("<PageUp>",   std::bind(&MessageView::previousPage, &_messageView));
    addHandledSequence("<C-u>",      std::bind(&MessageView::previousPage, &_messageView));

    addHandledSequence("gg",         std::bind(&MessageView::moveToTop, &_messageView));
    addHandledSequence("<Home>",     std::bind(&MessageView::moveToTop, &_messageView));
    addHandledSequence("G",          std::bind(&MessageView::moveToBottom, &_messageView));
    addHandledSequence("<End>",      std::bind(&MessageView::moveToBottom, &_messageView));

    addHandledSequence("r",          std::bind(&ThreadView::reply, &_threadView));

    addHandledSequence(" ",          std::bind(&ThreadMessageView::nextMessage, this));
    addHandledSequence("<C-n>",      std::bind(&ThreadMessageView::nextMessage, this));
    addHandledSequence("<C-p>",      std::bind(&ThreadMessageView::previousMessage, this));
}

ThreadMessageView::~ThreadMessageView()
{
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

void ThreadMessageView::resize(const View::Geometry & geometry)
{
    _threadView.resize({ geometry.x, geometry.y, geometry.width, threadViewHeight });
    _messageView.resize({
        geometry.x, threadViewHeight + 1,
        geometry.width, geometry.height - threadViewHeight - 1
    });
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
    _messageView.moveToTop();
}

void ThreadMessageView::loadSelectedMessage()
{
    _messageView.setMessage(_threadView.selectedMessage().id);
}

std::vector<std::string> ThreadMessageView::status() const
{
    std::vector<std::string> mergedStatus(_threadView.status());
    const std::vector<std::string> & messageViewStatus(_messageView.status());
    std::copy(messageViewStatus.begin(), messageViewStatus.end(), std::back_inserter(mergedStatus));

    return mergedStatus;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

