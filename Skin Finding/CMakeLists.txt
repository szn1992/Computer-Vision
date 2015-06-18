cmake_minimum_required (VERSION 2.6)
project (cse455au14hw3)

set(CMAKE_SUPPRESS_REGENERATION TRUE)

add_executable(kmeans kmeans.cpp utils.cpp utils.h)
add_executable(autocolor autocolor.cpp utils.cpp utils.h)
add_executable(arff arff.cpp utils.cpp utils.h)
add_executable(arff_begin arff_begin.cpp utils.cpp utils.h)
add_executable(prediction prediction.cpp utils.cpp utils.h)
add_executable(accuracy accuracy.cpp utils.cpp utils.h)
