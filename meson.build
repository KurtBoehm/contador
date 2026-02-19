# This file is part of https://github.com/KurtBoehm/contador.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

project('contador', 'cpp', default_options: ['cpp_std=c++23', 'warning_level=3'])

cc = meson.get_compiler('cpp')
dl_dep = cc.find_library('dl', required: false)

if target_machine.system() == 'linux'
  contador_lib = library(
    'contador',
    'src/tracing.cpp',
    include_directories: 'include',
    dependencies: dl_dep,
  )
  contador_dep = declare_dependency(
    include_directories: 'include',
    link_with: contador_lib,
  )

  contador_reduced_lib = library(
    'contador-reduced',
    'src/tracing-reduced.cpp',
    include_directories: 'include',
    dependencies: dl_dep,
  )
  contador_reduced_dep = declare_dependency(
    include_directories: 'include',
    link_with: contador_reduced_lib,
  )
endif

contador_dummy_lib = library(
  'contador-dummy',
  'src/dummy.cpp',
  include_directories: 'include',
)
contador_dummy_dep = declare_dependency(
  include_directories: 'include',
  link_with: contador_dummy_lib,
)

if get_option('include_tests')
  subdir('test')
endif
