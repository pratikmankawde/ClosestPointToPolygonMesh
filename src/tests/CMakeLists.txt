cmake_minimum_required(VERSION 3.10.2)
 
enable_testing()
 
configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Link runTests with what we want to test and the GTest and pthread library

file(GLOB TESTSOURCES "*.cpp")

add_executable(ClosestPointTests ${TESTSOURCES})

target_include_directories(ClosestPointTests PUBLIC
  $<BUILD_INTERFACE:${CMAKE_HOME_DIRECTORY}/src/include>
  $<INSTALL_INTERFACE:include> 
)


target_link_libraries(ClosestPointTests
ClosestPointLib
gtest_main
pthread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

install(TARGETS ClosestPointTests DESTINATION ${CMAKE_BINARY_DIR}/bin)