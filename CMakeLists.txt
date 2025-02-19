cmake_minimum_required(VERSION 3.13)

macro(force_set_flags var these those)
  if("${${var}}" MATCHES "${these}")
    string(REGEX REPLACE "${these}" "${those}" ${var} "${${var}}")
  else()
    set(${var} "${${var}} ${those}")
  endif()
endmacro()

macro(replace_flags var these those)
  if("${${var}}" MATCHES "${these}")
    string(REGEX REPLACE "${these}" "${those}" ${var} "${${var}}")
  endif()
  message(STATUS "info: ${var}='${${var}}'")
endmacro()

macro(msvc_link_to_static_crt)
  if(MSVC)
    set(has_correct_flag 0)
    foreach(lang C CXX)
    foreach(suffix "" _DEBUG _MINSIZEREL _RELEASE _RELWITHDEBINFO)
      replace_flags("CMAKE_${lang}_FLAGS${suffix}" "/MD" "/MT")
      if(CMAKE_${lang}_FLAGS${suffix} MATCHES "/MT")
        set(has_correct_flag 1)
      endif()
    endforeach()
    endforeach()
    if(NOT has_correct_flag)
      message(FATAL_ERROR "no CMAKE_*_FLAGS var contains /MT")
    endif()
  endif()
endmacro()

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
add_executable(${TESTS_PROJECT_NAME} EXCLUDE_FROM_ALL ${TESTS_SRC})
add_dependencies(${PROJECT_NAME} ${TESTS_PROJECT_NAME})
