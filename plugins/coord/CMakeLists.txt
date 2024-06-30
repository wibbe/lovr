cmake_minimum_required(VERSION 3.5.0)
project(coord)

add_library(coord MODULE coord.c)
set_target_properties(coord PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
#target_include_directories(lua-enet PRIVATE enet/include)
#target_link_libraries(lua-enet enet)

set_target_properties(coord PROPERTIES PREFIX "")
set_target_properties(coord PROPERTIES OUTPUT_NAME "coord")

