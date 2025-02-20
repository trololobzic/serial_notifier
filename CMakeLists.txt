cmake_minimum_required(VERSION 3.13)
include(${CMAKE_CURRENT_SOURCE_DIR}/macro.cmake)

project(serial_notifier)
set(CMAKE_MFC_FLAG 1)
#set warning level
force_set_flags(CMAKE_CXX_FLAGS "/W[0-4]" "/W4")
force_set_flags(CMAKE_CXX_FLAGS_DEBUG "/W[0-4]" "/W3")
force_set_flags(CMAKE_C_FLAGS "/W[0-4]" "/W4")
force_set_flags(CMAKE_C_FLAGS_DEBUG "/W[0-4]" "/W3")

#set optimization level
force_set_flags(CMAKE_CXX_FLAGS_RELEASE "/O[0-4,x]" "/Ox")
force_set_flags(CMAKE_C_FLAGS_RELEASE "/O[0-4,x]" "/Ox")

FIND_PACKAGE(MFC)
IF (NOT MFC_FOUND)
  MESSAGE(FATAL_ERROR "MFC Could not be found during the MFC test")
ENDIF()

file(GLOB_RECURSE SRCS "src/*.cpp")
file(GLOB_RECURSE HEARDERS "src/*.h")
file(GLOB_RECURSE RESOURSES "src/*.rc" "src/*.rc2")

if("${CMAKE_MFC_FLAG}" STREQUAL "1")
  msvc_link_to_static_crt()
else()
  # VS generators add this automatically based on the CMAKE_MFC_FLAG value,
  # but generators matching "Make" require:
  add_definitions(-D_AFXDLL)
endif()

create_directories_structure(src "Source")
add_executable(${PROJECT_NAME} WIN32 ${SRCS} ${HEARDERS} ${RESOURSES})

if("${CMAKE_MFC_FLAG}" STREQUAL "2")
  set(CMAKE_INSTALL_MFC_LIBRARIES ON)
  include(InstallRequiredSystemLibraries)
endif()

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)


#Tests project in the same solution
set(TESTS_PROJECT_NAME "${PROJECT_NAME}_tests")
file(GLOB_RECURSE TESTS_SRC "tests/*.cpp" "tests/*.h" "tests/*.hpp")
create_directories_structure(tests "Tests")
add_executable(${TESTS_PROJECT_NAME} EXCLUDE_FROM_ALL ${TESTS_SRC})
add_dependencies(${PROJECT_NAME} ${TESTS_PROJECT_NAME})
