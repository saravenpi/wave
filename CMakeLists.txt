cmake_minimum_required(VERSION 3.5)

project(Wave)

set(CMAKE_C_FLAGS "-Wall -I.")

set(SRCS src/wave.c src/route.c src/connection.c src/response.c src/request.c)
set(OBJS src/wave.o src/route.o src/connection.o src/response.o src/request.o)

set(NAME wave)
set(LIBRARY libwave.so)

add_library(${LIBRARY} SHARED ${SRCS})
set_target_properties(${LIBRARY} PROPERTIES OUTPUT_NAME ${NAME})
target_compile_options(${LIBRARY} PRIVATE ${CMAKE_C_FLAGS})

