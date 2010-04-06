/* ner: src/message_view.hh
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

#ifndef NER_MESSAGE_VIEW_H
#define NER_MESSAGE_VIEW_H 1

#include <string>
#include <vector>

#include "email_view.hh"

class MessageView : public EmailView
{
    public:
        class InvalidMessageException : public std::exception
        {
            public:
                InvalidMessageException(const std::string & messageId);
                ~InvalidMessageException() throw();

                virtual const char * what() const throw();

            private:
                std::string _id;
        };

        MessageView(const View::Geometry & geometry = View::Geometry());
        virtual ~MessageView();

        void setMessage(const std::string & messageId);

        virtual std::string name() const { return "message-view"; }
        virtual Type type() const { return Type::MessageView; }
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

