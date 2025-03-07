project('contador', 'cpp', default_options: ['cpp_std=c++23', 'warning_level=3'])

fmt_dep = dependency('fmt')

contador_lib = library(
  'contador',
  'src/contador.cpp',
  include_directories: 'include',
  dependencies: fmt_dep,
  cpp_args: ['-DCONTADOR_TRACING=true'],
)
contador_dep = declare_dependency(
  include_directories: 'include',
  link_with: contador_lib,
)

contador_dummy_lib = library(
  'contador-dummy',
  'src/contador.cpp',
  include_directories: 'include',
  dependencies: fmt_dep,
  cpp_args: ['-DCONTADOR_TRACING=false'],
)
contador_dummy_dep = declare_dependency(
  include_directories: 'include',
  link_with: contador_dummy_lib,
)
