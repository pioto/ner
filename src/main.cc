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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <clocale>
#include <unistd.h>
#include <gmime/gmime.h>
#include <yaml.h>

#include "notmuch.hh"
#include "ner.hh"
#include "view_manager.hh"
#include "search_list_view.hh"
#include "identity_manager.hh"

#define NOTMUCH_CONFIG_FILE ".notmuch-config"
#define NER_CONFIG_FILE ".ner.yaml"

void loadConfig()
{
    std::string configPath(std::string(getenv("HOME")) + "/" NER_CONFIG_FILE);
    std::ifstream configFile(configPath.c_str());

    YAML::Parser parser(configFile);

    YAML::Node document;
    parser.GetNextDocument(document);

    IdentityManager::instance().load(document.FindValue("identities"));
    IdentityManager::instance().setDefaultIdentity(*document.FindValue("default_identity"));
}

int main(int argc, char * argv[])
{
    std::setlocale(LC_ALL, "");

    srand(time(NULL));
    g_mime_init(0);

    const char * environmentConfigPath = std::getenv("NOTMUCH_CONFIG");
    std::string defaultConfigPath(std::string(std::getenv("HOME")) + "/" NOTMUCH_CONFIG_FILE);

    const std::string & configPath = (environmentConfigPath != NULL &&
        access(environmentConfigPath, R_OK) == 0) ? environmentConfigPath : defaultConfigPath;

    NotMuch::setConfig(configPath);

    loadConfig();

    Ner ner;

    std::shared_ptr<View> searchListView(new SearchListView());
    ner.viewManager()->addView(searchListView);
    ner.run();

    g_mime_shutdown();

    return EXIT_SUCCESS;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

