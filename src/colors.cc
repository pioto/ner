/* ner: src/colors.cc
 *
 * Copyright (c) 2012 Michael Forney
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

#include <ncurses.h>

#include "colors.hh"

std::map<ColorID, Color> defaultColorMap = {
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
    { ColorID::SearchListViewResults,   Color{ COLOR_GREEN,  COLOR_BLACK } },

    /* Message Parts */
    { ColorID::AttachmentFilename,      Color{ COLOR_YELLOW,  COLOR_BLACK } },
    { ColorID::AttachmentMimeType,      Color{ COLOR_MAGENTA, COLOR_BLACK } },
    { ColorID::AttachmentFilesize,      Color{ COLOR_GREEN,   COLOR_BLACK } },

    /* Citation levels */
    { ColorID::CitationLevel1, Color{ COLOR_GREEN,   COLOR_BLACK } },
    { ColorID::CitationLevel2, Color{ COLOR_YELLOW,  COLOR_BLACK } },
    { ColorID::CitationLevel3, Color{ COLOR_CYAN,    COLOR_BLACK } },
    { ColorID::CitationLevel4, Color{ COLOR_MAGENTA, COLOR_BLACK } }
};

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

