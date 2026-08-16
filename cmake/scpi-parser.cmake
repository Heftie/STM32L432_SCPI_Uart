#
# Builds the scpi-parser library (https://github.com/j123b567/scpi-parser)
# vendored as a git submodule under ./scpi-parser (kept out of Middlewares/,
# which STM32CubeMX code generation may wipe and regenerate).
#

set(SCPI_PARSER_DIR ${CMAKE_SOURCE_DIR}/scpi-parser/libscpi)

add_library(scpi-parser STATIC
    ${SCPI_PARSER_DIR}/src/error.c
    ${SCPI_PARSER_DIR}/src/expression.c
    ${SCPI_PARSER_DIR}/src/fifo.c
    ${SCPI_PARSER_DIR}/src/ieee488.c
    ${SCPI_PARSER_DIR}/src/lexer.c
    ${SCPI_PARSER_DIR}/src/minimal.c
    ${SCPI_PARSER_DIR}/src/parser.c
    ${SCPI_PARSER_DIR}/src/units.c
    ${SCPI_PARSER_DIR}/src/utils.c
)

target_include_directories(scpi-parser PUBLIC ${SCPI_PARSER_DIR}/inc)
