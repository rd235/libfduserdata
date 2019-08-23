cmake_minimum_required(VERSION 3.0)
project(fduserdata
		VERSION 0.0.1
		DESCRIPTION "associate file descriptors with user defined data"
		HOMEPAGE_URL "https://github.com/rd235/libfduserdata")

include(GNUInstallDirs)
add_definitions(-D_GNU_SOURCE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(HEADERS_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/include)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(FDUSERDATA_SOURCES fduserdata.c)
add_library(fduserdata SHARED ${FDUSERDATA_SOURCES})
target_link_libraries(fduserdata pthread)

set_target_properties(fduserdata PROPERTIES VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})

configure_file(fduserdata.pc.in fduserdata.pc @ONLY)

install(TARGETS fduserdata LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES fduserdata.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/fduserdata.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

add_subdirectory(man)
