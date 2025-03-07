project('contador', 'cpp', default_options: ['cpp_std=c++23', 'warning_level=3'])

fmt_dep = dependency('fmt')

contador_lib = library(
  'contador',
  'src/tracing.cpp',
  include_directories: 'include',
  dependencies: fmt_dep,
)
contador_dep = declare_dependency(
  include_directories: 'include',
  link_with: contador_lib,
)

contador_dummy_lib = library(
  'contador-dummy',
  'src/dummy.cpp',
  include_directories: 'include',
  dependencies: fmt_dep,
)
contador_dummy_dep = declare_dependency(
  include_directories: 'include',
  link_with: contador_dummy_lib,
)
