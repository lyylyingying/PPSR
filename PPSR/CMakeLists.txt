cmake_minimum_required(VERSION 3.10)

# project name
project(SHEImageSearch)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# Find OpenSSL library
find_package(OpenSSL REQUIRED)

# Set source files
set(SOURCE_FILES
    src/main.cpp
    src/she.cpp
    src/aes.cpp
    src/kmeans.cpp
    src/HMACH.cpp
    src/read_csv_ZZ.cpp
    src/utils.cpp
)

# Include header directories
include_directories(include)

# Add executable
add_executable(main ${SOURCE_FILES})

# Link libraries
find_library(NTL_LIB ntl REQUIRED)
find_library(GMP_LIB gmp REQUIRED)
target_link_libraries(main OpenSSL::SSL OpenSSL::Crypto ${NTL_LIB} ${GMP_LIB})
