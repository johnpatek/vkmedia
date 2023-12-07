cmake_minimum_required(VERSION 3.0)

find_package(PkgConfig) 
pkg_search_module(GLIB REQUIRED IMPORTED_TARGET glib-2.0) 
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GST_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
pkg_check_modules(GST_RTSP REQUIRED IMPORTED_TARGET gstreamer-rtsp-1.0)
pkg_check_modules(GST_RTSP_SERVER REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0)

project(vkserver)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(src)

add_subdirectory(example)