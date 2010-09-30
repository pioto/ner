/* ner: src/maildir.cc
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

#include <unistd.h>
#include <sstream>
#include <fstream>

#include "maildir.hh"

int Maildir::deliveries = 0;

Maildir::Maildir(const std::string & path)
    : _path(path)
{
}

Maildir::~Maildir()
{
}

bool Maildir::addMessage(GMimeMessage * message)
{
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    std::ostringstream uniqueName;
    uniqueName << time(NULL) << '.' << 'P' << getpid() << 'Q' << deliveries++ << '.' << hostname << ":2,S";

    std::string tmpUniquePath(_path + "/tmp/" + uniqueName.str());
    std::string newUniquePath(_path + "/new/" + uniqueName.str());

    FILE * tmpMessageFile = fopen(tmpUniquePath.c_str(), "w");

    if (!tmpMessageFile)
        return false;

    GMimeStream * stream = g_mime_stream_file_new(tmpMessageFile);
    g_mime_object_write_to_stream(GMIME_OBJECT(message), stream);
    g_object_unref(stream);

    link(tmpUniquePath.c_str(), newUniquePath.c_str());
    unlink(tmpUniquePath.c_str());

    return true;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

