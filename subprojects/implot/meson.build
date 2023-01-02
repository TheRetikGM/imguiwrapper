project('implot', 'cpp', 
  license : 'MIT',
  version : 'd87512353495e7760e7fda7566a05beef7627d8f')

include_dirs = include_directories('.')
sources = files(
  'implot.cpp',
  'implot_demo.cpp',
  'implot_items.cpp'
)

depends = [
  dependency('imgui')
]

implot = library('implot',
  sources,
  dependencies : depends,
  include_directories : include_dirs
)

implot_dep = declare_dependency(
  link_with : implot,
  include_directories : include_dirs,
  dependencies : depends
)

meson.override_dependency('implot', implot_dep)
