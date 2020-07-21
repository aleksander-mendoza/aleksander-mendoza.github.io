cmake_minimum_required(VERSION 3.10)

# set the project name and language
project(FinalMealyCompiler LANGUAGES C VERSION 0.1 )
# Add Bison and Flex support
find_package(BISON)
find_package(FLEX)
# Tell bison where to find grammar files
BISON_TARGET(SyntaxParser syntax_parser.y ${CMAKE_CURRENT_BINARY_DIR}/syntax_parser.c)
# Tell Flex where to find tokenizer files
FLEX_TARGET(SyntaxScanner syntax_scanner.l  ${CMAKE_CURRENT_BINARY_DIR}/syntax_scanner.c COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/lex.yy.h )
# We added COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/lex.yy.h
# because we want to make reentrant parser and this time we will make the header dependency more explicit
ADD_FLEX_BISON_DEPENDENCY(SyntaxScanner SyntaxParser)
# add the executable
add_executable(final_mealy_compiler
	ast.c
	glushkov.c
    include/ast.h
    include/glushkov.h
    include/hashmap.h
    ${BISON_SyntaxParser_OUTPUTS}
    ${FLEX_SyntaxScanner_OUTPUTS}
)
target_include_directories(final_mealy_compiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(final_mealy_compiler PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
# Set C version
set_property(TARGET final_mealy_compiler PROPERTY C_STANDARD 11)
# This will help you with debugging errors
set(CMAKE_VERBOSE_MAKEFILE ON)

get_target_property(OUT final_mealy_compiler LINK_LIBRARIES)
message(STATUS ${OUT})
