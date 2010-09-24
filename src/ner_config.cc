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
#include "colors.hh"
#include "ncurses.hh"

const std::string nerConfigFile(".ner.yaml");

struct Color
{
    int foreground;
    int background;
};

void operator>>(const YAML::Node & node, Color & color)
{
    static const std::map<std::string, int> ncursesColors = {
        { "black",      COLOR_BLACK },
        { "red",        COLOR_RED },
        { "green",      COLOR_GREEN },
        { "yellow",     COLOR_YELLOW },
        { "blue",       COLOR_BLUE },
        { "magenta",    COLOR_MAGENTA },
        { "cyan",       COLOR_CYAN },
        { "white",      COLOR_WHITE }
    };

    std::string foreground = *node.FindValue("fg");
    std::string background = *node.FindValue("bg");

    color.foreground = ncursesColors.at(foreground);
    color.background = ncursesColors.at(background);
}

void operator>>(const YAML::Node & node, Search & search)
{
    node.FindValue("name")->Read(search.name);
    node.FindValue("query")->Read(search.query);
}

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

    /* Identities */
    IdentityManager::instance().load(document.FindValue("identities"));

    const YAML::Node * defaultIdentity = document.FindValue("default_identity");
    if (defaultIdentity)
        IdentityManager::instance().setDefaultIdentity(*defaultIdentity);

    /* Commands */
    const YAML::Node * commands = document.FindValue("commands");
    if (commands)
        commands->Read(_commands);

    /* Saved Searches */
    const YAML::Node * searches = document.FindValue("searches");
    if (searches)
        searches->Read(_searches);
    else
        _searches = {
            { "New", "tag:inbox and tag:unread" },
            { "Unread", "tag:unread" },
            { "Inbox", "tag:inbox" }
        };

    /* Colors */
    std::map<ColorID, Color> colorMap = {
        /* General */
        { ColorID::CutOffIndicator,         Color{ COLOR_GREEN,  COLOR_BLACK } },
        { ColorID::MoreLessIndicator,       Color{ COLOR_BLACK,  COLOR_GREEN } },
        { ColorID::EmptySpaceIndicator,     Color{ COLOR_CYAN,   COLOR_BLACK } },
        { ColorID::LineWrapIndicator,       Color{ COLOR_GREEN,  COLOR_BLACK } },

        /* Status Bar */
        { ColorID::StatusBarStatus,         Color{ COLOR_WHITE,  COLOR_BLUE } },
        { ColorID::StatusBarStatusDivider,  Color{ COLOR_WHITE,  COLOR_BLUE } },
        { ColorID::StatusBarMessage,        Color{ COLOR_BLACK,  COLOR_WHITE } },
        { ColorID::StatusBarPrompt,         Color{ COLOR_WHITE,  COLOR_BLACK } },

        /* Search View */
        { ColorID::SearchViewDate,                  Color{ COLOR_YELLOW,     COLOR_BLACK } },
        { ColorID::SearchViewMessageCountComplete,  Color{ COLOR_GREEN,      COLOR_BLACK } },
        { ColorID::SearchViewMessageCountPartial,   Color{ COLOR_MAGENTA,    COLOR_BLACK } },
        { ColorID::SearchViewAuthors,               Color{ COLOR_CYAN,       COLOR_BLACK } },
        { ColorID::SearchViewSubject,               Color{ COLOR_WHITE,      COLOR_BLACK } },
        { ColorID::SearchViewTags,                  Color{ COLOR_RED,        COLOR_BLACK } },

        /* ThreadView */
        { ColorID::ThreadViewArrow, Color{ COLOR_GREEN,  COLOR_BLACK } },
        { ColorID::ThreadViewDate,  Color{ COLOR_CYAN,   COLOR_BLACK } },
        { ColorID::ThreadViewTags,  Color{ COLOR_RED,    COLOR_BLACK } },

        /* Email View */
        { ColorID::EmailViewHeader, Color{ COLOR_CYAN, COLOR_BLACK } },

        /* View View */
        { ColorID::ViewViewNumber,  Color{ COLOR_CYAN,   COLOR_BLACK } },
        { ColorID::ViewViewName,    Color{ COLOR_GREEN,  COLOR_BLACK } },
        { ColorID::ViewViewStatus,  Color{ COLOR_WHITE,  COLOR_BLACK } },

        /* Search List View */
        { ColorID::SearchListViewName,      Color{ COLOR_CYAN,   COLOR_BLACK } },
        { ColorID::SearchListViewTerms,     Color{ COLOR_YELLOW, COLOR_BLACK } },
        { ColorID::SearchListViewResults,   Color{ COLOR_GREEN,  COLOR_BLACK } }
    };

    const YAML::Node * colors = document.FindValue("colors");
    if (colors)
    {
        std::map<std::string, ColorID> colorNames = {
            /* General */
            { "cut_off_indicator",      ColorID::CutOffIndicator },
            { "more_less_indicator",    ColorID::MoreLessIndicator },
            { "empty_space_indicator",  ColorID::EmptySpaceIndicator },
            { "line_wrap_indicator",    ColorID::LineWrapIndicator },

            /* Status Bar */
            { "status_bar_status",          ColorID::StatusBarStatus },
            { "status_bar_status_divider",  ColorID::StatusBarStatusDivider },
            { "status_bar_message",         ColorID::StatusBarMessage },
            { "status_bar_prompt",          ColorID::StatusBarPrompt },

            /* Search View */
            { "search_view_date",                   ColorID::SearchViewDate },
            { "search_view_message_count_complete", ColorID::SearchViewMessageCountComplete },
            { "search_view_message_count_partial",  ColorID::SearchViewMessageCountPartial },
            { "search_view_authors",                ColorID::SearchViewAuthors },
            { "search_view_subject",                ColorID::SearchViewSubject },
            { "search_view_tags",                   ColorID::SearchViewTags },

            /* Thread View */
            { "thread_view_arrow",  ColorID::ThreadViewArrow },
            { "thread_view_date",   ColorID::ThreadViewDate },
            { "thread_view_tags",   ColorID::ThreadViewTags },

            /* Email View */
            { "email_view_header",  ColorID::EmailViewHeader },

            /* View View */
            { "view_view_number",   ColorID::ViewViewNumber },
            { "view_view_name",     ColorID::ViewViewName },
            { "view_view_status",   ColorID::ViewViewStatus },

            /* Search List View */
            { "search_list_view_name",      ColorID::SearchListViewName },
            { "search_list_view_terms",     ColorID::SearchListViewTerms },
            { "search_list_view_results",   ColorID::SearchListViewTerms }
        };

        for (auto name = colors->begin(), e = colors->end(); name != e; ++name)
            colorMap[colorNames.at(name.first())] = name.second();
    }

    for (auto color = colorMap.begin(), e = colorMap.end(); color != e; ++color)
        init_pair(color->first, color->second.foreground, color->second.background);
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

const std::vector<Search> & NerConfig::searches() const
{
    return _searches;
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

