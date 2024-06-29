cmake_minimum_required(VERSION 3.5)

project(Wave)

set(CMAKE_C_FLAGS "-Wall")

set(SRC wave.c)
set(OBJ wave.o)
set(NAME wave)
set(LIBRARY libwave.so)


add_library(${LIBRARY} SHARED ${SRC})
set_target_properties(${LIBRARY} PROPERTIES OUTPUT_NAME ${NAME})
target_compile_options(${LIBRARY} PRIVATE ${CMAKE_C_FLAGS})

