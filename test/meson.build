# This file is part of https://github.com/KurtBoehm/contador.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

mt_exe = executable(
  'multithread',
  'multithread.cpp',
  dependencies: [contador_dummy_dep],
)
test(
  'multithread-dummy',
  mt_exe,
)
test(
  'multithread-full',
  mt_exe,
  env: {'LD_PRELOAD': contador_lib.full_path()},
)
test(
  'multithread-reduced',
  mt_exe,
  env: {'LD_PRELOAD': contador_reduced_lib.full_path()},
)
