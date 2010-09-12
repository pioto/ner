/* ner: src/input_handler.hh
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

#ifndef NER_INPUT_HANDLER_H
#define NER_INPUT_HANDLER_H 1

#include <vector>
#include <map>
#include <functional>
#include <string>

/**
 * Accepts input sequences to perform different actions.
 */
class InputHandler
{
    public:
        /**
         * The result of handling a key sequence.
         */
        enum class HandleResult
        {
            NoMatch,
            PartialMatch,
            Handled
        };

        virtual ~InputHandler() = 0;

        /**
         * Attempt to handle a key sequence.
         *
         * If the input handler is able sucessfully able to handle the given
         * input sequence, HandleResult::Handled will be returned and the action
         * associated with that sequence will be run.
         *
         * If the key sequence is a partial match to one of the handled key
         * sequences, HandleResult::Partial match will be returned. This
         * indicates that the current input buffer should be kept.
         *
         * Otherwise, HandleResult::NoMatch is returned, indicating that the
         * current input buffer should be discarded.
         *
         * \param sequence The key sequence to handle.
         * \return How the sequence was handled
         */
        virtual HandleResult handleKeySequence(const std::vector<int> & sequence);

    protected:
        /**
         * Add a new sequence to the set of handled key sequences.
         *
         * Strings surrounded in angle braces will be interpreted with parseKey.
         *
         * \param string The sequence of keys to handle
         * \param function The function to execute when the sequence is executed
         */
        void addHandledSequence(const std::string & string, const std::function<void ()> & function);

        /**
         * Parses the special key into an integer for ncurses
         *
         * Examples:
         *   - "C-a"    : Control 'a'
         *   - "Home"   : Home
         *   - "S-Home" : Shift Home
         */
        int parseKey(const std::string & keyString) const;

    private:
        std::map<std::vector<int>, std::function<void ()>> _handledSequences;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

