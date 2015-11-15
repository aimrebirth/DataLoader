#
# Polygon-4 Data generator
#

BISON_TARGET(grammar
    "${CMAKE_CURRENT_SOURCE_DIR}/grammar.yy"
    "${CMAKE_CURRENT_BINARY_DIR}/grammar.cpp"
)
FLEX_TARGET(lexer
    "${CMAKE_CURRENT_SOURCE_DIR}/lexer.ll"
    "${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp"
    COMPILE_FLAGS --header-file="${CMAKE_CURRENT_BINARY_DIR}/lexer.h"
)
ADD_FLEX_BISON_DEPENDENCY(lexer grammar)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(parser_src
    ${BISON_grammar_OUTPUTS}
    ${FLEX_lexer_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.h
)

file(GLOB data_gen_src "*.cpp" "*.h")

set(data_gen_src ${data_gen_src}
    grammar.yy
    lexer.ll
    ../data/schema.txt
    ${parser_src}
)

add_executable(data_gen ${data_gen_src})
