add_definitions(-DSYSYC_UNITTEST)

set(SYSYC_TEST_SOURCE
  src/unit-lexer.cc
  src/unit-parser.cc
  src/unit-evaluator.cc
  src/unit-codegen-sysy-ast.cc
  src/unit-codegen-eeyore-quad.cc
  src/unit-codegen-tigger-quad.cc
  src/unit-codegen-riscv-quad.cc
)

foreach(src_file ${SYSYC_TEST_SOURCE})
  get_filename_component(src_filename ${src_file} NAME)
  string(REGEX REPLACE "^unit-(.+)\\.cc$" "\\1" test_case ${src_filename})
  set(exec "test-${test_case}")

  add_executable(${exec} ${src_file})

  target_include_directories(${exec} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/)

  target_link_libraries(${exec} sysyc_static)

  add_test(NAME ${test_case}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${exec}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endforeach()
