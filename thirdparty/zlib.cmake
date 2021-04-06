
include(FindPkgConfig)

add_library(zlib INTERFACE)
# find_package(zlib ${ZLIB_VERSION})

if(NOT ZLIB_FOUND)
	set(SYSTEM_ZLIB FALSE)
	include(ExternalProject)
	include(GNUInstallDirs)

	set(ZLIB_PREFIX "zlib-${ZLIB_VERSION}")

	if(NOT DEFINED ZLIB_URL)
		set(ZLIB_URL ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib1211.zip)
	endif()
	if(NOT DEFINED ZLIB_SRC_BASE)
		set(ZLIB_SRC_BASE ${CMAKE_CURRENT_BINARY_DIR}/${ZLIB_PREFIX}) 
	endif()
	set(ZLIB_URL_MD5 16b41357b2cd81bca5e1947238e64465)

	ExternalProject_Add(${ZLIB_PREFIX}
			PREFIX ${ZLIB_PREFIX}
			URL ${ZLIB_URL}
			URL_MD5 ${ZLIB_URL_MD5}
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 0
			LOG_BUILD 0
			CONFIGURE_COMMAND ${ZLIB_SRC_BASE}/src/${ZLIB_PREFIX}/configure
				--prefix=${CMAKE_CURRENT_BINARY_DIR}
				--includedir ${CMAKE_CURRENT_BINARY_DIR}/include
				--libdir ${CMAKE_CURRENT_BINARY_DIR}/lib 
			BUILD_COMMAND make install	
	)

	add_dependencies(zlib ${ZLIB_PREFIX})
    set(ZLIB_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}z${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include")
	set(ZLIB_FOUND TRUE)
else()
	set(SYSTEM_ZLIB TRUE)
endif()

target_link_libraries(zlib INTERFACE ${ZLIB_LIBRARIES})
target_include_directories(zlib INTERFACE ${ZLIB_INCLUDE_DIRS})

