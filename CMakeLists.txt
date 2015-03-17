project(tinylog)

enable_testing()

cmake_minimum_required(VERSION 2.6)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

add_library(tinyarg STATIC src/tinyarg.c)
add_executable(tinyarg_test tests/tinyarg_test.c)
add_test(tinyarg_testsuite tinyarg_test)

install(TARGETS tinyarg DESTINATION lib)
install(FILES src/tinyarg.h DESTINATION include)
