project('ImGui Wrapper', 'cpp',
  version : 'docking',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

inc = include_directories('include')

deps = [
  dependency('imgui'),
  subproject('glad', default_options : ['default_library=static']).get_variable('glad_dep'),
  dependency('glfw3', static : (build_machine.system() == 'windows')),
  dependency('nlohmann_json', required : get_option('nlohmann_json')),
  dependency('glm', required : get_option('glm')),
  dependency('implot', required : get_option('implot')),
  dependency('xtensor', required : get_option('xtensor')),
  dependency('gl')
]

if get_option('yaml-cpp').enabled()
  cmake = import('cmake')
  opt_var = cmake.subproject_options()
  opt_var.set_override_option('cpp_std', 'c++17', target : 'yaml-cpp' )
  opt_var.set_override_option('cpp_std', 'c++17', target : 'gmock')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'gmock_main')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'gtest')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'gtest_main')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'yaml-cpp-parse')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'yaml-cpp-read')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'yaml-cpp-sandbox')
  opt_var.set_override_option('cpp_std', 'c++17', target : 'yaml-cpp-tests')
  deps += cmake.subproject('yaml-cpp', options : opt_var).dependency('yaml-cpp')
endif

if get_option('build-examples')
  if build_machine.system() != 'windows'
    exe = executable('example', ['src/example.cpp'],
      dependencies : [deps, dependency('nlohmann_json', required : true)],
      include_directories : inc)
  endif
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
