cmake_minimum_required(VERSION 3.12)
project(fduserdata
		VERSION 0.1.2
		DESCRIPTION "associate file descriptors with user defined data"
		HOMEPAGE_URL "https://github.com/rd235/libfduserdata"
		LANGUAGES C)

include(GNUInstallDirs)
add_definitions(-D_GNU_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if (PROJECT_INCLUDEDIR)
       include_directories(${PROJECT_INCLUDEDIR})
       execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/fduserdata.h ${PROJECT_INCLUDEDIR}/.)
endif (PROJECT_INCLUDEDIR)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(FDUSERDATA_SOURCES fduserdata.c)
add_library(fduserdata SHARED ${FDUSERDATA_SOURCES})
target_link_libraries(fduserdata pthread)

set_target_properties(fduserdata PROPERTIES VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})

configure_file(fduserdata.pc.in fduserdata.pc @ONLY)

install(TARGETS fduserdata LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES fduserdata.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fduserdata.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

add_subdirectory(man)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${PROJECT_SOURCE_DIR}/Uninstall.cmake")
