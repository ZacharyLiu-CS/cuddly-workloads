#1. specify the version of cmake
cmake_minimum_required(VERSION 3.10)

#2. define the name of project
project(tpcc_workload)

#3. specify c++ compiler version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -g -pthread -O3 ")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-DON_DCPMM)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPMEMKV REQUIRED libpmemkv)
pkg_check_modules(LIBPMEMOBJ REQUIRED libpmemobj)
pkg_check_modules(LIBPMEM REQUIRED libpmem)

#4. head file path
include_directories(
  ${PROJECT_SOURCE_DIR}/tpcc
  ${PROJECT_SOURCE_DIR}/utils
  ${PROJECT_SOURCE_DIR}/db_impl/pmem-rocksdb/include
  ${PROJECT_SOURCE_DIR}/db_impl/listdb/
  ${PROJECT_SOURCE_DIR}/db_impl/NeoPMKV/build/neopmkv/include
  ${LIBPMEMKV_INCLUDE_DIRS}
  ${LIBPMEMOBJ_INCLUDE_DIRS}
  )

#5. third path link path
link_directories(
  ${PROJECT_SOURCE_DIR}/db_impl/pmem-rocksdb/
  ${PROJECT_SOURCE_DIR}/db_impl/listdb/build
  ${PROJECT_SOURCE_DIR}/db_impl/NeoPMKV/build/neopmkv/lib
  ${LIBPMEM_LIBRARY_DIRS}
  ${LIBPMEMKV_LIBRARY_DIRS}
  ${LIBPMEMOBJ_LIBRARY_DIRS}
  )

#6. source code file path
file(GLOB MAIN_FILE
  ${PROJECT_SOURCE_DIR}/tpcc.cc
  )
file(GLOB SOURCE_FILE
  ${PROJECT_SOURCE_DIR}/tpcc/*.cc
  )
list(FILTER SOURCE_FILE EXCLUDE REGEX ".*_test.cc")
list(REMOVE_ITEM SOURCE_FILE ${MAIN_FILE})

#7. define the executable
add_executable(${PROJECT_NAME} ${MAIN_FILE} ${SOURCE_FILE})
target_link_libraries(${PROJECT_NAME}
  ${LIBPMEMOBJ_LIBRARIES} 
  pthread
  gflags
  rocksdb
  pmemobj
  pmem
  pthread
  pmemkv
  snappy
  zstd
  lz4
  bz2
  z
  dl
  )


#9. compile test files
find_package(GTest REQUIRED)
find_package(gflags REQUIRED)
enable_testing()
if (GTEST_FOUND)
  file(GLOB TEST_FILE ${PROJECT_SOURCE_DIR}/tpcc/*_test.cc)
  foreach(testsourcefile ${TEST_FILE})
    string(REGEX MATCH "[^/]+$" testsourcefilewithoutpath ${testsourcefile})
    string(REPLACE ".cc" "" testname ${testsourcefilewithoutpath})
    add_executable( ${testname} ${testsourcefile} ${SOURCE_FILE})
    target_link_libraries(${testname}
      ${LIBPMEMOBJ_LIBRARIES}
      rocksdb
      pmemobj
      pmem
      pthread
      gtest
      gflags
      snappy
      zstd
      lz4
      bz2
      z
      dl
      )
    add_test(NAME ${testname} COMMAND ${testname})
  ENDFOREACH(testsourcefile ${TEST_FILE})
endif()

