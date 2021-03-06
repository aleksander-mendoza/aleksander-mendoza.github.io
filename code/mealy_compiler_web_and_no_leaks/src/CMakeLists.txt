cmake_minimum_required(VERSION 3.10)

# set the project name and language
project(WebMealyCompiler LANGUAGES C VERSION 0.1 )
# Add Bison and Flex support
find_package(BISON)
find_package(FLEX)
# Tell bison where to find grammar files
BISON_TARGET(SyntaxParser syntax_parser.y ${CMAKE_CURRENT_BINARY_DIR}/syntax_parser.c)
# Tell Flex where to find tokenizer files
FLEX_TARGET(SyntaxScanner syntax_scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/syntax_scanner.c)
ADD_FLEX_BISON_DEPENDENCY(SyntaxScanner SyntaxParser)
# add the executable
add_executable(web_mealy_compiler
    main.c
    include/main.h
    ${BISON_SyntaxParser_OUTPUTS}
    ${FLEX_SyntaxScanner_OUTPUTS}
)
# Optionally add directories to include, but we won't actually use it here.
target_include_directories(web_mealy_compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Set C version
set_property(TARGET web_mealy_compiler PROPERTY C_STANDARD 11)
# This will help you with debugging errors
set(CMAKE_VERBOSE_MAKEFILE ON)


add_definitions("-s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\",\"allocate\",\"intArrayFromString\"]'")
add_definitions("-s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\",\"allocate\",\"intArrayFromString\"]'")
add_definitions("-s EXPORTED_FUNCTIONS='[\"_compile\",\"_testInt\",\"_run_global\"]'")
set_target_properties(web_mealy_compiler PROPERTIES LINK_FLAGS "-s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap','allocate','intArrayFromString']")
# 
get_target_property(OUT web_mealy_compiler LINK_LIBRARIES)
message(STATUS ${OUT})
