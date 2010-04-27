/* ner: src/compose_view.cc
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

#include <iostream>
#include <cstring>
#include <gmime/gmime.h>

#include "compose_view.hh"
#include "notmuch.hh"

ComposeView::ComposeView(const View::Geometry & geometry)
    : EmailEditView(geometry)
{
    GMimeMessage * message = g_mime_message_new(true);

    std::string to = StatusBar::instance().prompt("To: ");
    std::string cc = StatusBar::instance().prompt("Cc: ");
    std::string bcc = StatusBar::instance().prompt("Bcc: ");
    std::string subject = StatusBar::instance().prompt("Subject: ");

    InternetAddress * from = internet_address_mailbox_new(
        g_key_file_get_string(NotMuch::config(), "user", "name", NULL),
        g_key_file_get_string(NotMuch::config(), "user", "primary_email", NULL)
    );
    g_mime_message_set_sender(message, internet_address_to_string(from, true));
    g_mime_object_set_header(GMIME_OBJECT(message), "To", to.c_str());
    g_mime_object_set_header(GMIME_OBJECT(message), "Cc", cc.c_str());
    g_mime_object_set_header(GMIME_OBJECT(message), "Bcc", bcc.c_str());
    g_mime_message_set_subject(message, subject.c_str());

    createMessage(message);

    edit();
}

ComposeView::~ComposeView()
{
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

