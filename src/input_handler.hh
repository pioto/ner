/* ner: src/input_handler.hh
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

#ifndef NER_INPUT_HANDLER_H
#define NER_INPUT_HANDLER_H 1

#include <vector>
#include <map>
#include <functional>
#include <string>

class InputHandler
{
    public:
        InputHandler() = default;
        ~InputHandler() = default;

        virtual bool handleKeySequence(const std::vector<int> & sequence);

        void addHandledSequence(const std::vector<int> & sequence, const std::function<void ()> & function);
        void addHandledSequence(const std::string & string, const std::function<void ()> & function);
        void addHandledSequence(const int, const std::function<void ()> & function);

    protected:
        std::map<std::vector<int>, std::function<void ()>> _handledSequences;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

