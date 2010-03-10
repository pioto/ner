/* ner: src/config_file.cc
 *
 * Copyright (c) 2010 Michael Forney
 *
 * This file is a part of ner.
 *
 * ner is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2, as published by the Free
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
#include <fstream>
#include <sstream>
#include <limits>

#include "config_file.hh"

ConfigFile::ConfigFile(const std::string & configPath)
    : _configPath(configPath)
{
}

ConfigFile::~ConfigFile()
{
}

std::string ConfigFile::value(const std::string & group, const std::string & name, const std::string & default_value)
{
    std::string value = _config[group][name];

    if (value.empty())  return default_value;
    else                return value;
}

bool ConfigFile::parse()
{
    std::ifstream stream;
    std::stringbuf input;
    std::string group, name, value;
    std::map<std::string, std::string> keyValueMap;

    stream.open(_configPath.c_str());

    stream >> std::skipws;

    while (stream.good())
    {
        /* Ignore comments */
        switch (stream.peek())
        {
            case '#':
            case '\n':
                stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
        }

        stream.get();
        input.str(std::string());
        stream.get(input, ']');
        stream.get();
        group = input.str();

        keyValueMap.clear();
        while (stream.good())
        {
            if (stream.peek() == '[' || stream.peek() < 0) break;
            if (stream.peek() == '#' || stream.peek() == '\n')
            {
                stream.ignore(256, '\n');
                continue;
            }

            input.str(std::string());
            stream.get(input, '=');
            stream.get();
            name = input.str();
            input.str(std::string());
            stream.get(input);
            value = input.str();

            /* Get the newline character */
            stream.get();

            keyValueMap.insert(std::pair<std::string, std::string>(name, value));
        }

        _config.insert(std::pair<std::string,
            std::map<std::string, std::string> >(group, keyValueMap));
    }

    stream.close();

    return true;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

