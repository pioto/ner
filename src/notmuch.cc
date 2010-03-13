/* ner: src/notmuch.cc
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

#include "notmuch.hh"

notmuch_database_t * NotMuch::_database = NULL;

notmuch_database_t * NotMuch::database()
{
    return _database;
}

void NotMuch::openDatabase(const std::string & path)
{
    _database = notmuch_database_open(path.c_str(), NOTMUCH_DATABASE_MODE_READ_ONLY);
}

void NotMuch::closeDatabase()
{
    notmuch_database_close(_database);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

