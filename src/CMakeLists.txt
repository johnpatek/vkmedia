file(GLOB VKMEDIA_SOURCES vkmedia/*.*)

add_library(vkmedia SHARED ${VKMEDIA_SOURCES})

target_include_directories(vkmedia PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(vkmedia PkgConfig::GST_RTSP_SERVER vulkan nvidia-encode cuda)

