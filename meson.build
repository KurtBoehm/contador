project('mylloc', 'cpp', default_options: ['cpp_std=c++23', 'warning_level=3'])

fmt_dep = dependency('fmt')
library('mylloc', 'src/alloc.cpp', dependencies: fmt_dep)
