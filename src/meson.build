spdlog_dep = dependency('spdlog', required: true)

# Сборка
spb = executable(
  'spb',
  [
    'spb.cpp', 'ids.cpp', 'xmltools.cpp', 'typing.cpp', 'objects.cpp'
  ],
  link_with: [argparse_lib, pugixml_lib, uuidv4_lib],
  include_directories: [argparse_inc, pugixml_inc, uuidv4_inc],
  dependencies: [spdlog_dep],
  cpp_args: '-march=native'
)
