argparse_inc = include_directories('./argparse/include')
argparse_lib = library(
    'argparse',
    './argparse/include/argparse/argparse.hpp'
)

pugixml_inc = include_directories('./pugixml/src')
pugixml_lib = library(
    'pugixml',
    './pugixml/src/pugixml.cpp',
    include_directories: pugixml_inc
)

uuidv4_inc = include_directories('./uuid_v4')
uuidv4_lib = library(
    'uuidv4',
    ['./uuid_v4/uuid_v4.h', './uuid_v4/endianness.h'],
    include_directories: uuidv4_inc,
    cpp_args: '-march=native'
)
