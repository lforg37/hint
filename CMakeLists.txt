cmake_minimum_required(VERSION 3.10)
project(Hint
		VERSION 0.4.0
	DESCRIPTION "A type safe compatibility layer between major HLS tools"
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(FetchContent)

#set(CPACK_PACKAGE_NAME "libhint")
#set(CPACK_PACKAGE_VENDOR "(Florent de Dinechin, Luc Forget, Yohann Uguen)@[Citi Lab - INSA Lyon - INRIA]  David Thomas@Imperial College London")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Boost 1.63.0 REQUIRED) 


set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(hint INTERFACE)
target_include_directories(
	hint INTERFACE
	$<BUILD_INTERFACE:${INCLUDE_ROOT}>
	$<INSTALL_INTERFACE:include/hint>
)
target_link_libraries(hint INTERFACE Boost::boost)

install(DIRECTORY ${INCLUDE_ROOT}/ DESTINATION include/hint FILES_MATCHING PATTERN "*.hpp" PATTERN "*.ipp")
install(
	TARGETS hint
	EXPORT ${PROJECT_NAME}Targets
	PUBLIC_HEADER DESTINATION "include/hint"
	)
install(FILES ${PROJECT_NAME}Config.cmake DESTINATION lib/cmake/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}Targets FILE ${PROJECT_NAME}Targets.cmake DESTINATION lib/cmake/${PROJECT_NAME})

find_package(APInt)

try_compile(SupportedBitInt "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/config/test_bitint.cpp")
if (${SupportedBitInt} STREQUAL "TRUE")
message(STATUS "Your compiler supports _BitInt")
  add_library(HINT_BITINT_BACKEND INTERFACE IMPORTED GLOBAL)
  target_compile_definitions(HINT_BITINT_BACKEND INTERFACE BITINT_BACKEND)
  target_link_libraries(HINT_BITINT_BACKEND INTERFACE hint)
  set(BITINT_BACKEND "Present")
  try_compile(SupportedSigneBitInt1 "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/config/test_signed_bitint_1.cpp")
  if (${SupportedSigneBitInt1} STREQUAL "TRUE")
  message(STATUS "It even accept width 1 signed _BitInt, congratulation")
  target_compile_definitions(HINT_BITINT_BACKEND INTERFACE BITINT_BACKEND_SIGNED_W1)
  endif() 
else()
message(STATUS "Your compiler does not support _BitInt")
endif()

if(APInt_FOUND)
    add_library(HINT_VIVADO_BACKEND INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(HINT_VIVADO_BACKEND INTERFACE VIVADO_BACKEND)
	target_link_libraries(HINT_VIVADO_BACKEND INTERFACE hint APInt::APInt)
else()
    set(VIVADO_BACKEND OFF CACHE BOOL "Is vivado backend available" FORCE)
    message(STATUS "Vivado include path unset please set VIVADO_INCLUDES if you intend to use Vivado HLS backend" )
endif()

set(INTEL_INCLUDES ${INTEL_INCLUDES} CACHE PATH "Path to INTEL ac_int includes")
if(EXISTS ${INTEL_INCLUDES}/ref/ac_int.h)
    set(INTEL_BACKEND ON)
    message(STATUS "Using ${INTEL_INCLUDES} as intelHLS include directory")
else()
        message(STATUS "IntelHLS include path unset please set INTEL_INCLUDES if you intend to use Intel HLS backend" )
endif()

set(CATAPULT_INCLUDES ${CATAPULT_INCLUDES} CACHE PATH "Path to CATAPULT ac_int includes")
if(EXISTS ${CATAPULT_INCLUDES}/ac_int.h)
    set(CATAPULT_BACKEND ON)
    message(STATUS "Using ${CATAPULT_INCLUDES} as CatapultHLS include directory")
else()
    message(STATUS "CatapultHLS include path unset please set CATAPULT_INCLUDES if you intend to use Catapult HLS backend" )
endif()

OPTION(BUILD_HINT_UNIT_TEST "Build the operators unit tests")
if(BUILD_HINT_UNIT_TEST)
    enable_testing()
    find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)
    add_subdirectory(tests)
endif()

#include(CPack)
