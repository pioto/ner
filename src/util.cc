/* ner: src/util.cc
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

#include "util.hh"

#define MINUTE (60)
#define HOUR (60 * MINUTE)
#define DAY (60 * HOUR)

std::string relativeTime(time_t rawTime)
{
    time_t currentRawTime = time(0);
    struct tm currentLocalTime, localTime;
    char timeString[13];

    if (rawTime > currentRawTime)
        return "the future";

    localtime_r(&currentRawTime, &currentLocalTime);
    localtime_r(&rawTime, &localTime);

    time_t difference = currentRawTime - rawTime;

    if (difference > 180 * DAY)
    {
        strftime(timeString, sizeof(timeString), "%F", &localTime);
    }
    else if (difference < HOUR)
    {
        snprintf(timeString, sizeof(timeString), "%u mins. ago", difference / MINUTE);
    }
    else if (difference < 7 * DAY)
    {
        if (localTime.tm_wday == currentLocalTime.tm_wday)
        {
            strftime(timeString, sizeof(timeString), "Today %R", &localTime);
        }
        else if ((currentLocalTime.tm_wday + 7 - localTime.tm_wday) % 7 == 1)
        {
            strftime(timeString, sizeof(timeString), "Yest. %R", &localTime);
        }
        else
        {
            strftime(timeString, sizeof(timeString), "%a. %R", &localTime);
        }
    }
    else
    {
        strftime(timeString, sizeof(timeString), "%B %d", &localTime);
    }

    return std::string(timeString);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

