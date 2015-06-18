cmake_minimum_required (VERSION 2.6)
project (cse455au14hw6)

set(CMAKE_SUPPRESS_REGENERATION TRUE)

add_executable(kmeans kmeans.cpp utils.cpp utils.h)
add_executable(autocolor autocolor.cpp utils.cpp utils.h)
add_executable(merge merge.cpp utils.cpp utils.h)
add_executable(computeDist computeDist.cpp utils.cpp utils.h)
add_executable(attributes attributes.cpp utils.cpp utils.h)
add_executable(query query.cpp utils.cpp utils.h)
add_executable(conrgn conrgn.cpp utils.cpp utils.h)
add_executable(erode erode.cpp utils.cpp utils.h)
add_executable(dilate dilate.cpp utils.cpp utils.h)