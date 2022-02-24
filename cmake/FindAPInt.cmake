# Distributed under the FloPoCo License, see README.md for more information

#[=======================================================================[.rst:
FindAPInt
-------

Finds the APInt library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``APInt::APInt``
  The APInt library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``APInt_FOUND``
  True if the system has the APInt library.
``APInt_VERSION``
  The version of the APInt library which was found.
``APInt_INCLUDE_DIRS``
  Include directories needed to use APInt.
``APInt_LIBRARIES``
  Libraries needed to link to APInt.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``APInt_INCLUDE_DIR``
  The directory containing ``sollya.h``.

#]=======================================================================]
if ((NOT VIVADO_INCLUDES) OR NOT (EXISTS ${VIVADO_INCLUDES}/ap_int.h))
  find_path(APInt_INCLUDE_DIR
    NAMES ap_int.h
    PATHS ${PC_APInt_INCLUDE_DIRS}
    DOC "Path of ap_int.h, the include file for GNU APInt library"
  )
else()
  set(APInt_INCLUDE_DIR ${VIVADO_INCLUDES})
endif()

if (NOT APInt_INCLUDE_DIR)
  include(FetchContent)
  FetchContent_Declare(XilinxAPInt
    GIT_REPOSITORY https://github.com/Xilinx/HLS_arbitrary_Precision_Types.git
    GIT_TAG 200a9aecaadf471592558540dc5a88256cbf880f
  )
  FetchContent_MakeAvailable(XilinxAPInt)
  set(APInt_INCLUDE_DIR "${xilinxapint_SOURCE_DIR}/include")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  APInt
  FOUND_VAR APInt_FOUND
  REQUIRED_VARS
    APInt_INCLUDE_DIR
)

if(APInt_FOUND)
  set(APInt_LIBRARIES)
  set(APInt_INCLUDE_DIRS ${APInt_INCLUDE_DIR})
  set(APInt_DEFINITIONS)
endif()

if (APInt_FOUND AND NOT TARGET APInt::APInt)
  add_library(APInt::APInt INTERFACE IMPORTED)
  set_target_properties(APInt::APInt PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${PC_APInt_FLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${APInt_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(APInt_INCLUDE_DIR APInt_LIBRARY)
