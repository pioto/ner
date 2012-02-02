/* ner: src/identity_manager.hh
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

#ifndef NER_IDENTITY_MANAGER_H
#define NER_IDENTITY_MANAGER_H 1

#include <string>
#include <memory>
#include <yaml.h>

#include "mail_store.hh"

struct Identity
{
    std::string name;
    std::string email;
    std::string signaturePath;
    std::string sendCommand;
    std::shared_ptr<MailStore> sentMail;
};

class IdentityManager
{
    public:
        static IdentityManager & instance();

        void load(const YAML::Node * node);
        void setDefaultIdentity(const std::string & identity);

        const Identity * defaultIdentity() const;
        const Identity * findIdentity(InternetAddress * address);
        const Identity * findIdentity(const std::string & name);

    private:
        IdentityManager();
        ~IdentityManager();

        std::map<std::string, Identity> _identities;
        std::string _defaultIdentity;
};

#endif

// vim: fdm=syntax fo=croql et sw=4 sts=4 ts=8

