/* ner: src/config_file.hh
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

#ifndef NER_CONFIG_FILE
#define NER_CONFIG_FILE 1

#include <string>
#include <map>

class ConfigFile
{
    public:
        ConfigFile(const std::string & configPath);
        ~ConfigFile();

        bool parse();
        std::string value(const std::string & group, const std::string & name,
            const std::string & default_value = std::string());

    private:
        std::string _configPath;
        std::map<std::string, std::map<std::string, std::string> > _config;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

