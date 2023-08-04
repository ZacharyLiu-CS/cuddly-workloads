#1. specify the version of cmake
cmake_minimum_required(VERSION 3.10)

#2. define the name of project
project(tpcc_workload)

#3. specify c++ compiler version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -g -Wall -pthread ")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#4. head file path
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/../utils
  )

#5. third path link path
link_directories(
  ${PROJECT_SOURCE_DIR}
  )

#6. source code file path
file(GLOB MAIN_FILE
  ${PROJECT_SOURCE_DIR}/tpcc.cc
  )
file(GLOB SOURCE_FILE
  ${PROJECT_SOURCE_DIR}/*.cc
  )
list(FILTER SOURCE_FILE EXCLUDE REGEX ".*_test.cc")
list(REMOVE_ITEM SOURCE_FILE ${MAIN_FILE})

#7. define the executable
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${SOURCE_FILE})
target_link_libraries(${PROJECT_NAME}
  gflags
  )


#9. compile test files
find_package(GTest REQUIRED)
find_package(gflags REQUIRED)
enable_testing()
if (GTEST_FOUND)
  file(GLOB TEST_FILE ${PROJECT_SOURCE_DIR}/*_test.cc)
  foreach(testsourcefile ${TEST_FILE})
    string(REGEX MATCH "[^/]+$" testsourcefilewithoutpath ${testsourcefile})
    string(REPLACE ".cc" "" testname ${testsourcefilewithoutpath})
    add_executable( ${testname} ${testsourcefile} ${SOURCE_FILE})
    target_link_libraries(${testname}
      gtest
      )
    add_test(NAME ${testname} COMMAND ${testname})
  ENDFOREACH(testsourcefile ${TEST_FILE})
endif()

