project('glad', 'c',
  version : '0.1',
  default_options : ['warning_level=0'])

glad_inc = include_directories('include')

glad_lib = library('glad', 'src/glad.c', include_directories : glad_inc)

glad_dep = declare_dependency(
  include_directories : glad_inc,
  link_with : glad_lib
)

