project('contador', 'cpp', default_options: ['cpp_std=c++23', 'warning_level=3'])

fmt_dep = dependency('fmt')
library(
  'contador',
  'src/contador.cpp',
  include_directories: 'include',
  dependencies: fmt_dep,
  cpp_args: ['-DCONTADOR_TRACING=true'],
)
library(
  'contador-dummy',
  'src/contador.cpp',
  include_directories: 'include',
  dependencies: fmt_dep,
  cpp_args: ['-DCONTADOR_TRACING=false'],
)
