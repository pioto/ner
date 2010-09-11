/* ner: src/ner_config.cc
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

#include <fstream>
#include <yaml.h>

#include "ner_config.hh"
#include "identity_manager.hh"

const std::string nerConfigFile(".ner.yaml");

NerConfig & NerConfig::instance()
{
    static NerConfig * config = NULL;

    if (!config)
        config = new NerConfig();

    return *config;
}

NerConfig::NerConfig()
{
}

NerConfig::~NerConfig()
{
}

void NerConfig::load()
{
    _commands.clear();

    std::string configPath(std::string(getenv("HOME")) + "/" + nerConfigFile);
    std::ifstream configFile(configPath.c_str());

    YAML::Parser parser(configFile);

    YAML::Node document;
    parser.GetNextDocument(document);

    IdentityManager::instance().load(document.FindValue("identities"));

    const YAML::Node * defaultIdentity = document.FindValue("default_identity");
    if (defaultIdentity)
        IdentityManager::instance().setDefaultIdentity(*defaultIdentity);

    const YAML::Node * commands = document.FindValue("commands");
    if (commands)
        commands->Read(_commands);
}

std::string NerConfig::command(const std::string & name)
{
    auto command = _commands.find(name);

    if (command == _commands.end())
    {
        /* Use a default */
        if (name == "send")
            return "/usr/sbin/sendmail -t";
        else if (name == "edit")
            return "vim +";
    }
    else
    {
        return command->second;
    }
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

