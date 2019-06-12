cmake_minimum_required (VERSION 3.10.2)

set (Boost_NO_BOOST_CMAKE ON)
set (CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/include/eigen3/)

find_package(Boost 1.70.0 REQUIRED) 
find_package(Eigen3 REQUIRED)  

project (ClosestPoint
		VERSION "1.0.0"
		LANGUAGES CXX)

include_directories(${Boost_INCLUDEDIR}) 

file(GLOB SOURCE_LIST "data/*.cpp")
file(GLOB HEADER_LIST "include/data/*.h")
file(GLOB TEST_LIST "tests/*.cpp")
 
list(APPEND HEADER_LIST ${SOURCE_LIST} ${TEST_LIST})
string(REPLACE ";" " " SOURCES "${HEADER_LIST}")
 
add_executable(ClosestPoint 
main.cpp
${SOURCE_LIST})

target_link_libraries(ClosestPoint ${Boost_LIBRARIES})

target_include_directories(ClosestPoint PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include> 
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

install(TARGETS ClosestPoint DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

install(DIRECTORY include
		DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        
#add_subdirectory(tests)
