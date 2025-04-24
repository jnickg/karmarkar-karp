cmake_minimum_required(VERSION 3.10)
project(KarmarkarKarp VERSION 0.1.0 LANGUAGES C CXX)

add_executable(KarmarkarKarp KarmarkarKarp/KarmarkarKarp.c)
target_link_libraries(KarmarkarKarp m)

add_executable(ldm LDMGPU/ldm.cpp)