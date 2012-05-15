/* ner: src/email_edit_view.hh
 *
 * Copyright (c) 2010 Michael Forney
 * Copyright (c) 2011 Maxime Coste
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

#ifndef NER_EMAIL_EDIT_VIEW_H
#define NER_EMAIL_EDIT_VIEW_H 1

#include "email_view.hh"
#include "identity_manager.hh"

class EmailEditView : public EmailView
{
    public:
        EmailEditView(const View::Geometry & geometry = View::Geometry());
        virtual ~EmailEditView();

        void edit();

    protected:
        /**
         * Creates a new message file at a temporary location using the
         * specified message.
         */
        virtual void createMessage(GMimeMessage * message);

        /**
         * Sends the message with the configured MTA
         */
        virtual void send();

        /**
         * Prompt for a filename and add it to attached files
         */
        void attach();

        /**
         * Remove currently selected attachment
         */
        void removeSelectedAttachment();

        /**
         * Sets the identity used for sending the message. If the specified
         * identity cannot be found, the identity will not be changed.
         */
        void setIdentity(const std::string & name);

        std::string _messageFile;
        const Identity * _identity;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

