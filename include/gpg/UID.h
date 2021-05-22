/**
 * This file is part of GPGFrontend.
 *
 * GPGFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from gpg4usb-team.
 * Their source code version also complies with GNU General Public License.
 *
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 */

#ifndef GPGFRONTEND_UID_H
#define GPGFRONTEND_UID_H

#include <utility>

#include "GpgFrontend.h"

#include "Signature.h"

struct UID {

    QString name{};

    QString email{};

    QString comment{};

    QString uid{};

    QVector<Signature> signatures;

    UID() = default;

    explicit UID(gpgme_user_id_t user_id):
            uid(user_id->uid), name(user_id->name), email(user_id->email), comment(user_id->comment) {

        auto sig = user_id->signatures;

        while (sig != nullptr) {
            signatures.push_back(Signature(sig));
        }

    }
};

#endif //GPGFRONTEND_UID_H