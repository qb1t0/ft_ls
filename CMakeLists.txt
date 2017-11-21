cmake_minimum_required(VERSION 3.8)
project(ft_ls)
set(CMAKE_C_STANDARD 99)
include_directories(include libft) # header directories
link_directories(libft) # library directories
set(SOURCE_FILES main.c lsq.h)
add_executable(ft_ls ${SOURCE_FILES})
target_link_libraries(ft_ls ${CMAKE_SOURCE_DIR}/libft/libft.a)