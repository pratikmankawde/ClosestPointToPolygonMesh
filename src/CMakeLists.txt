cmake_minimum_required (VERSION 3.10.2)

set (Boost_NO_BOOST_CMAKE ON)
set (CMAKE_CXX_STANDARD 11)

find_package(Boost 1.70.0 REQUIRED) 
find_package(Eigen3 REQUIRED)  
find_library(tbb REQUIRED) 

include_directories(${Boost_INCLUDEDIR}) 

file(GLOB_RECURSE DATA_LIST "data/*.cpp")
file(GLOB_RECURSE HEADER_LIST "include/*.h")
file(GLOB_RECURSE TEST_LIST "tests/*.cpp")
 
#list(APPEND HEADER_LIST ${DATA_LIST} ${TEST_LIST})
#string(REPLACE ";" " " SOURCES "${HEADER_LIST}")

add_library(ClosestPointLib
STATIC
main.cpp
${DATA_LIST})

target_link_libraries(ClosestPointLib
${Boost_LIBRARIES}
tbb)

target_include_directories(ClosestPointLib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include> 
)

add_executable(ClosestPoint 
main.cpp
${DATA_LIST})

target_link_libraries(ClosestPoint
${Boost_LIBRARIES}
tbb)

target_include_directories(ClosestPoint PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include> 
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

install(TARGETS ClosestPoint DESTINATION ${CMAKE_BINARY_DIR}/bin)
install(TARGETS ClosestPointLib ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/bin)
      
add_subdirectory(tests)