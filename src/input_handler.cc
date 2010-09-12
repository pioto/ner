/* ner: src/input_handler.cc
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

#include <stdexcept>

#include "input_handler.hh"
#include "ncurses.hh"

InputHandler::~InputHandler()
{
}

InputHandler::HandleResult InputHandler::handleKeySequence(const std::vector<int> & sequence)
{
    auto lowerBound = _handledSequences.lower_bound(sequence);

    /* If there are no keys greater than or equal to sequence */
    if (lowerBound == _handledSequences.end())
        return HandleResult::NoMatch;
    /* If there is an exact match */
    else if (sequence == (*lowerBound).first)
    {
        (*lowerBound).second();

        return HandleResult::Handled;
    }
    /* If there is a partial match */
    else if (std::equal(sequence.begin(), sequence.end(), (*lowerBound).first.begin()))
        return HandleResult::PartialMatch;
    /* Otherwise there is no match */
    else
        return HandleResult::NoMatch;
}

void InputHandler::addHandledSequence(const std::string & string, const std::function<void ()> & function)
{
    std::vector<int> sequence;

    for (auto key = string.begin(), e = string.end(); key != e; ++key)
    {
        if (*key == '<')
        {
            auto keyEnd = std::find(key, e, '>');

            ++key;
            sequence.push_back(parseKey(std::string(key, keyEnd)));
            key = keyEnd;
        }
        else
            sequence.push_back(*key);
    }

    _handledSequences[sequence] = function;
}

int InputHandler::parseKey(const std::string & keyString) const
{
    if (keyString.size() == 3 && keyString.substr(0, 2) == "C-")
        return keyString[2] - 96;
    else if (keyString.size() > 2 && keyString.substr(0, 2) == "S-")
    {
        std::map<std::string, int> shiftedKeyMap = {
            { "Delete",     KEY_SDC },
            { "End",        KEY_SEND },
            { "Home",       KEY_SHOME },
            { "Left",       KEY_SLEFT },
            { "Right",      KEY_SRIGHT },
            { "Backspace",  KEY_BACKSPACE },
        };

        auto key = shiftedKeyMap.find(keyString.substr(2, 0));

        if (key == shiftedKeyMap.end())
            throw std::runtime_error("Unknown special key: " + keyString);
        else
            return key->second;
    }
    else
    {
        std::map<std::string, int> keyMap = {
            { "Down",       KEY_DOWN },
            { "Up",         KEY_UP },
            { "Left",       KEY_LEFT },
            { "Right",      KEY_RIGHT },
            { "Home",       KEY_HOME },
            { "Backspace",  KEY_BACKSPACE },
            { "F1",         KEY_F(1) },
            { "F2",         KEY_F(2) },
            { "F3",         KEY_F(3) },
            { "F4",         KEY_F(4) },
            { "F5",         KEY_F(5) },
            { "F6",         KEY_F(6) },
            { "F7",         KEY_F(7) },
            { "F8",         KEY_F(8) },
            { "F9",         KEY_F(9) },
            { "F10",        KEY_F(10) },
            { "F11",        KEY_F(11) },
            { "F12",        KEY_F(12) },
            { "Delete",     KEY_DC },
            { "Insert",     KEY_IC },
            { "PageDown",   KEY_NPAGE },
            { "PageUp",     KEY_PPAGE },
            { "Enter",      KEY_ENTER },
            { "BackTab",    KEY_BTAB },
            { "End",        KEY_END },
        };

        auto key = keyMap.find(keyString);

        if (key == keyMap.end())
            throw std::runtime_error("Unknown special key: " + keyString);
        else
            return key->second;
    }
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

