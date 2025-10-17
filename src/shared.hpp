// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SRC_SHARED_HPP
#define SRC_SHARED_HPP

#ifdef __linux__
#define CONTADOR_GNU_WEAK [[gnu::weak]]
#else
#define CONTADOR_GNU_WEAK
#endif

#endif // SRC_SHARED_HPP
