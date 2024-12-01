project('imgui', 'cpp', 
	license : 'MIT', 
	version : 'git-14d213ca851b399fd54f4c6593a1235bfdb5c0c3', 
	meson_version : '>=0.54.0')

include_dirs = include_directories('.', 'backends')
sources = files(
  'misc/cpp/imgui_stdlib.cpp',
  'imgui.cpp',
  'imgui_demo.cpp',
  'imgui_draw.cpp',
  'imgui_tables.cpp',
  'imgui_widgets.cpp',
)

cpp = meson.get_compiler('cpp')
dependencies = []

# renderer backends
libgl_dep = dependency('gl', required: true)
if libgl_dep.found()
  sources += 'backends/imgui_impl_opengl3.cpp'
  dependencies += libgl_dep
  dependencies += cpp.find_library('dl', required: false)
endif

# platform backends
glfw_dep = dependency('glfw3', static : (build_machine.system() == 'windows'), required: true) 
if glfw_dep.found()
  sources += 'backends/imgui_impl_glfw.cpp'
  dependencies += glfw_dep
endif

api = []
if build_machine.system() != 'windows'
	api += '-DIMGUI_API=__attribute__((visibility("default")))'
endif

imgui = static_library(
  'imgui',
  sources,
  cpp_args: api,
  gnu_symbol_visibility: 'hidden',
  dependencies: dependencies,
  include_directories: include_dirs,
)

imgui_dep = declare_dependency(
  compile_args: api,
  include_directories: include_dirs,
  link_with: imgui,
)

meson.override_dependency('imgui', imgui_dep)
