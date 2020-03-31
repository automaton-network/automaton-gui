cmake_minimum_required(VERSION 3.1)
project(automaton-playground)

if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) # MSVC_RUNTIME_VERSION support
endif()
option(automaton_STATIC_RUNTIME "Use static C runtime" ON)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  option(automaton_USE_OPENSSL "Use OpenSSL" ON)
else()
  set(automaton_USE_OPENSSL ON)
endif()

set(automaton_PATH ${CMAKE_SOURCE_DIR}/../automaton/src/local_third_party CACHE FILEPATH "Directory with automaton")

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VENDOR "Automaton Network")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode)

file(
  GLOB PLAYGROUND_FILES
  Source/*.cpp
  Source/Components/*.cpp
  Source/Config/*.cpp
  Source/Data/*.cpp
  Source/Demos/*.cpp
  Source/Miner/*.cpp
  Source/Network/*.cpp
  Source/DEX/*.cpp
  Source/Proposals/*.cpp
  JuceLibraryCode/BinaryData.cpp
)

include_directories(${automaton_PATH}/include)
include_directories(${automaton_PATH}/JUCE/modules)

add_executable(Playground MACOSX_BUNDLE ${PLAYGROUND_FILES})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  if (automaton_STATIC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
  set(CRYPTOPP_LIB ${automaton_PATH}/lib/cryptopp-static.lib)
  set(PROTOBUF_LIB ${automaton_PATH}/lib/libprotobuf.lib)
  set(CURL_LIB ${automaton_PATH}/lib/libcurl.lib)
  set(BOOST_LIBS ${automaton_PATH}/lib/libboost_system.lib;${automaton_PATH}/lib/libboost_date_time.lib;${automaton_PATH}/lib/libboost_iostreams.lib;${automaton_PATH}/lib/libboost_filesystem.lib)
  if (automaton_USE_OPENSSL)
    set(OPENSSL_LIBS ${automaton_PATH}/lib/libssl.lib;${automaton_PATH}/lib/libcrypto.lib)
  else()
    set(OPENSSL_LIBS "")
  endif()
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(GTEST_LIBS ${automaton_PATH}/lib/gtestd.lib;${automaton_PATH}/lib/gtest_maind.lib)
    set(REPLXX_LIB ${automaton_PATH}/lib/replxx-d.lib)
    set(PROTOBUF_LIB ${automaton_PATH}/lib/libprotobufd.lib)
    set(CURL_LIB ${automaton_PATH}/lib/libcurl-d.lib)
    set(Z_LIB ${automaton_PATH}/lib/zlibstaticd.lib)
  else()
    set(GTEST_LIBS ${automaton_PATH}/lib/gtest.lib;${automaton_PATH}/lib/gtest_main.lib)
    set(REPLXX_LIB ${automaton_PATH}/lib/replxx.lib)
    set(PROTOBUF_LIB ${automaton_PATH}/lib/libprotobuf.lib)
    set(CURL_LIB ${automaton_PATH}/lib/libcurl.lib)
    set(Z_LIB ${automaton_PATH}/lib/zlibstatic.lib)
  endif()
  set(G3LOGGER_LIB ${automaton_PATH}/lib/g3logger.lib)
  set(LUA LIB ${automaton_PATH}/lib/lua.lib)
  set(AUTOMATON_CORE_LIB "${automaton_PATH}/lib/automaton-core.lib;${automaton_PATH}/lib/blockchain_cpp_node.lib;${automaton_PATH}/lib/secp256k1.lib;${automaton_PATH}/lib/ed25519.lib")
  target_compile_definitions(Playground PUBLIC
    _WINDOWS
    WIN32
  )
  # TODO(vitalyster): lua library have main() function,
  # that is the reason why autodetection failed
  set_target_properties(Playground PROPERTIES
    LINK_FLAGS /SUBSYSTEM:WINDOWS
  )
else()
  set(CRYPTOPP_LIB ${automaton_PATH}/lib/libcryptopp.a)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(PROTOBUF_LIB ${automaton_PATH}/lib/libprotobufd.a)
    set(GTEST_LIBS ${automaton_PATH}/lib/libgtestd.a;${automaton_PATH}/lib/libgtest_maind.a)
    set(CURL_LIB ${automaton_PATH}/lib/libcurl-d.a)
  else()
    set(PROTOBUF_LIB ${automaton_PATH}/lib/libprotobuf.a)
    set(GTEST_LIBS ${automaton_PATH}/lib/libgtest.a;${automaton_PATH}/lib/libgtest_main.a)
    set(CURL_LIB ${automaton_PATH}/lib/libcurl.a)
  endif()
  set(BOOST_LIBS ${automaton_PATH}/lib/libboost_system.a;${automaton_PATH}/lib/libboost_date_time.a;${automaton_PATH}/lib/libboost_iostreams.a;${automaton_PATH}/lib/libboost_filesystem.a)
  set(REPLXX_LIB ${automaton_PATH}/lib/libreplxx.a)
  set(OPENSSL_LIBS ${automaton_PATH}/lib/libssl.a;${automaton_PATH}/lib/libcrypto.a)
  set(Z_LIB ${automaton_PATH}/lib/libz.a)
  set(GMP_LIB ${automaton_PATH}/lib/libgmp.a)
  set(G3LOGGER_LIB ${automaton_PATH}/lib/libg3logger.a)
  set(LUA_LIB ${automaton_PATH}/lib/liblua.a)
  set(AUTOMATON_CORE_LIB "${automaton_PATH}/lib/libblockchain_cpp_node.a;${automaton_PATH}/lib/libautomaton-core.a;${automaton_PATH}/lib/libsecp256k1.a;${automaton_PATH}/lib/libed25519.a")
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  find_package(Freetype REQUIRED)
  find_package(X11 REQUIRED)
  target_include_directories(Playground PRIVATE
    FREETYPE_INCLUDE_DIRS
    X11_INCLUDE_DIR;X11_Xrandr_INCLUDE_PATH;X11_Xinerama_INCLUDE_PATH;X11_Xcursor_INCLUDE_PATH
  )
  target_link_libraries(Playground
    Freetype::Freetype
    ${X11_LIBRARIES}
  )
endif()

macro(add_juce_module target_name module_name)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_sources(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/include_juce_${module_name}.mm)
else()
  target_sources(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/include_juce_${module_name}.cpp)
endif()
endmacro()

add_juce_module(Playground core)
add_juce_module(Playground data_structures)
add_juce_module(Playground events)
add_juce_module(Playground graphics)
add_juce_module(Playground gui_basics)
add_juce_module(Playground gui_extra)

target_link_libraries(
  Playground
  ${AUTOMATON_CORE_LIB}
  Threads::Threads
  ${REPLXX_LIB}
  ${CRYPTOPP_LIB}
  ${G3LOGGER_LIB}
  ${PROTOBUF_LIB}
  ${CURL_LIB}
  ${LUA_LIB}
  ${BOOST_LIBS}
  ${OPENSSL_LIBS}
  ${Z_LIB}
  ${GTEST_LIBS}
  ${CMAKE_DL_LIBS}
  ${GMP_LIB}
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set_target_properties(Playground PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.Liberatix.PlaygroundGUI
    MACOSX_BUNDLE_NAME Playground
    MACOSX_BUNDLE_VERSION 0.0.1
    MACOSX_BUNDLE_ICON_FILE ${CMAKE_SOURCE_DIR}/Builds/MacOSX/Icon.icns
  )
  set(CMAKE_EXE_LINKER_FLAGS "-framework Cocoa -framework Foundation -framework IOKit -framework CoreImage -w")
endif()

# On Windows if using curl we need to also link to LDAP, Wincrypt, Winnls, Winsock.
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(
    Playground
    crypt32 Normaliz wldap32 ws2_32
  )
endif()

macro(automaton_configure_debugger_directory target_name)
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set_property(TARGET ${target_name} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endmacro()

automaton_configure_debugger_directory(Playground)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
target_compile_definitions(
  Playground PUBLIC
  CURL_STATICLIB
  BOOST_ALL_NO_LIB
  _WIN32_WINNT=0x0A00
)
endif()

install(TARGETS Playground
  RUNTIME
    DESTINATION bin
  BUNDLE
    DESTINATION bin
)
