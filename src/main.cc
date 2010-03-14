/* ner: src/main.cc
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

#include <stdlib.h>
#include <iostream>

#include "notmuch.hh"
#include "ner.hh"
#include "config_file.hh"
#include "view_manager.hh"
#include "search_view.hh"

#define NOTMUCH_CONFIG_FILE ".notmuch-config"

int main(int argc, char * argv[])
{
    std::string configPath(getenv("HOME"));
    configPath.append("/" NOTMUCH_CONFIG_FILE);

    ConfigFile configFile(configPath);
    configFile.parse();

    NotMuch::openDatabase(configFile.value("database", "path"));

    Ner ner;

    ner.viewManager()->addView(new SearchView("tag:inbox"));
    ner.run();

    return EXIT_SUCCESS;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

