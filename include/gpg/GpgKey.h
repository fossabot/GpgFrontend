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

#ifndef GPGFRONTEND_GPGKEY_H
#define GPGFRONTEND_GPGKEY_H

#include "GpgUID.h"
#include "GpgSubKey.h"

class GpgKey {
public:

    QString id;
    QString name;
    QString email;
    QString comment;
    QString fpr;
    QString protocol;
    QString owner_trust;
    QString pubkey_algo;
    QDateTime last_update;
    QDateTime expires;
    QDateTime create_time;

    unsigned int length{};

    bool can_encrypt{};
    bool can_sign{};
    bool can_certify{};
    bool can_authenticate{};


    bool is_private_key{};
    bool expired{};
    bool revoked{};
    bool disabled{};

    bool has_master_key{};

    bool good = false;

    QVector<GpgSubKey> subKeys;

    QVector<GpgUID> uids;

    gpgme_key_t key_refer = nullptr;

    explicit GpgKey(gpgme_key_t key = nullptr);

    GpgKey(const GpgKey &k);

    GpgKey(GpgKey &&k)  noexcept;

    GpgKey& operator=(const GpgKey &k);

    GpgKey& operator=(GpgKey &&k)  noexcept;

    ~GpgKey();

    void parse(gpgme_key_t key);

    void swapKeyRefer(gpgme_key_t key);
};


#endif //GPGFRONTEND_GPGKEY_H
