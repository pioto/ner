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

#include "input_handler.hh"

InputHandler::HandleResult InputHandler::handleKeySequence(const std::vector<int> & sequence)
{
    auto lowerBound = _handledSequences.lower_bound(sequence);

    /* If there are no keys greater than or equal to sequence */
    if (lowerBound == _handledSequences.end())
        return NO_MATCH;
    /* If there is an exact match */
    else if (sequence == (*lowerBound).first)
    {
        (*lowerBound).second();

        return HANDLED;
    }
    /* If there is a partial match */
    else if (std::equal(sequence.begin(), sequence.end(), (*lowerBound).first.begin()))
        return PARTIAL_MATCH;
    /* Otherwise there is no match */
    else
        return NO_MATCH;
}

void InputHandler::addHandledSequence(const std::vector<int> & sequence, const std::function<void ()> & function)
{
    _handledSequences[sequence] = function;
}

void InputHandler::addHandledSequence(const std::string & string, const std::function<void ()> & function)
{
    std::vector<int> sequence;

    for (auto i = string.begin(), e = string.end(); i != e; ++i)
    {
        sequence.push_back(*i);
    }

    addHandledSequence(sequence, function);
}

void InputHandler::addHandledSequence(const int key, const std::function<void ()> & function)
{
    std::vector<int> sequence;

    sequence.push_back(key);

    addHandledSequence(sequence, function);
}

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

