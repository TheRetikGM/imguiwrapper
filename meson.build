project('ImGui Wrapper', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

inc = include_directories('include')

deps = [
  dependency('imgui'),
  subproject('glad', default_options : ['default_library=static']).get_variable('glad_dep'),
  dependency('glfw3'),
  dependency('nlohmann_json', required : get_option('nlohmann_json')),
  dependency('glm', required : get_option('glm')),
  dependency('implot', required : get_option('implot'))
]

if get_option('yaml-cpp').enabled()
  cmake = import('cmake')
  deps += cmake.subproject('yaml-cpp').dependency('yaml-cpp')
endif

if get_option('build-examples')
  exe = executable('example', ['src/example.cpp'],
    dependencies : [deps, dependency('nlohmann_json', required : true)],
    include_directories : inc)
endif

cppargs = []
if get_option('implot').enabled()
  cppargs += '-DIMGUIWRAPPER_IMPLOT'
endif

imguiwrapper_inc = inc
imguiwrapper_dep = declare_dependency(
  compile_args : cppargs,
  dependencies : deps, 
  include_directories : inc
)
meson.override_dependency('imguiwrapper', imguiwrapper_dep)
