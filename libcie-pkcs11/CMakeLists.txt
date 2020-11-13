cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# ---- Project ----

# Note: update this to your new project's name and version
project(
  cie-pkcs11
  VERSION 1.3.1
  LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# ---- Create library ----

# Note: for header-only libraries change all PUBLIC flags to INTERFACE and create an interface
# target: add_library(cie-pkcs11 INTERFACE) set_target_properties(cie-pkcs11 PROPERTIES
# INTERFACE_COMPILE_FEATURES cxx_std_17)
set(CMAKE_CXX_FLAGS "-UWIN32 -U_WIN64 -I/usr/include/PCSC")
set(CMAKE_SHARED_LINKER_FLAGS "-lcryptopp -lcrypto -lpcsclite")
add_library(cie-pkcs11 SHARED ${headers} ${sources})

set_target_properties(cie-pkcs11 PROPERTIES CXX_STANDARD 17)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(cie-pkcs11 PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

# Link dependencies (if required) target_link_libraries(cie-pkcs11 PUBLIC cxxopts)

target_include_directories(
  cie-pkcs11 PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:include>
)
