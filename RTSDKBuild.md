
cmake -H/home/ramaswamy/externsrc/Real-Time-SDK -Bbuild   -DCMAKE_INSTALL_PREFIX=/home/ramaswamy/externsrc/Real-Time-SDK/run |& tee cmake.log1

Tell CMake to prefer system libraries RTSDK uses CMake ≥ 3.22. You can override dependency resolution with flags:
cmake -HRTSDK -BbuildDir \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_SYSTEM_OPENSSL=ON \
  -DUSE_SYSTEM_CURL=ON \
  -DUSE_SYSTEM_CJSON=ON \
  -DUSE_SYSTEM_LIBXML2=ON \
  -DUSE_SYSTEM_ZLIB=ON \
  -DUSE_SYSTEM_LZ4=ON \
  -DUSE_SYSTEM_GOOGLETEST=ON \
  -DUSE_SYSTEM_CCron=ON \
  -DUSE_SYSTEM_L8W8JWT=ON
(These flags are conventional; if RTSDK doesn’t define them, you can still force CMake to pick system libs by setting CMAKE_PREFIX_PATH or CMAKE_LIBRARY_PATH.)

Explicit paths if needed If your system has multiple versions installed, point CMake explicitly:
--------------------------------
-DOPENSSL_ROOT_DIR=/usr/lib64/openssl-3.0 \
-DCURL_INCLUDE_DIR=/usr/include/curl \
-DCURL_LIBRARY=/usr/lib64/libcurl.so
-------------
cmake -HRTSDK -BbuildDir \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/usr/local/openssl-3.0;/usr/local/curl;/usr/lib64;/usr/src/gtest" \
  -DOPENSSL_ROOT_DIR=/usr/local/openssl-3.0 \
  -DCURL_ROOT=/usr/local/curl \
  -DGTEST_ROOT=/usr/src/gtest
----------------------

Googletest special case

On Linux, libgtest-dev installs sources but not compiled libs. You may need to build once:

Takes System Path:
cmake -HRTSDK -BbuildDir \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/usr/local;/opt/libs"


Sample CMakeLists.txt

cmake_minimum_required(VERSION 3.22)
project(RTSDK_SystemLibs CXX)

# --- OpenSSL ---
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install system OpenSSL (>=1.1.1).")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# --- Curl ---
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found. Please install system curl (>=8.12.1).")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

# --- cJSON ---
find_path(CJSON_INCLUDE_DIR cJSON.h)
find_library(CJSON_LIBRARY cjson)
if(NOT CJSON_INCLUDE_DIR OR NOT CJSON_LIBRARY)
    message(FATAL_ERROR "cJSON not found. Please install system cJSON (>=1.7.18).")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${CJSON_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CJSON_LIBRARY})

# --- libxml2 ---
find_package(LibXml2 REQUIRED)
if(NOT LIBXML2_FOUND)
    message(FATAL_ERROR "libxml2 not found. Please install system libxml2 (>=2.13.8).")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE LibXml2::LibXml2)

# --- zlib ---
find_package(ZLIB REQUIRED)
if(NOT ZLIB_FOUND)
    message(FATAL_ERROR "zlib not found. Please install system zlib (>=1.3.1).")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

# --- lz4 ---
find_path(LZ4_INCLUDE_DIR lz4.h)
find_library(LZ4_LIBRARY lz4)
if(NOT LZ4_INCLUDE_DIR OR NOT LZ4_LIBRARY)
    message(FATAL_ERROR "lz4 not found. Please install system lz4 (>=1.9.4).")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${LZ4_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LZ4_LIBRARY})

# --- googletest ---
find_package(GTest REQUIRED)
if(NOT GTest_FOUND)
    message(FATAL_ERROR "googletest not found. Please install system gtest (>=1.12.1).")
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main)

# --- l8w8jwt ---
find_path(L8W8JWT_INCLUDE_DIR l8w8jwt.h)
find_library(L8W8JWT_LIBRARY l8w8jwt)
if(NOT L8W8JWT_INCLUDE_DIR OR NOT L8W8JWT_LIBRARY)
    message(FATAL_ERROR "l8w8jwt not found. Please install system l8w8jwt (>=2.5.0).")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${L8W8JWT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${L8W8JWT_LIBRARY})

# --- ccron ---
find_path(CCRON_INCLUDE_DIR ccron.h)
find_library(CCRON_LIBRARY ccron)
if(NOT CCRON_INCLUDE_DIR OR NOT CCRON_LIBRARY)
    message(FATAL_ERROR "ccron not found. Please install system ccron (>=2.0.0).")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${CCRON_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CCRON_LIBRARY})
