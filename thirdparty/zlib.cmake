include(FindPkgConfig)

add_library(zlib INTERFACE)
set(ZLIB_VERSION "1.2.11")
find_package(zlib ${ZLIB_VERSION})

if(NOT ZLIB_FOUND)
	include(ExternalProject)
	include(GNUInstallDirs)

	set(ZLIB_PREFIX "zlib-${ZLIB_VERSION}")

	set(ZLIB_URL ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib1211.zip)
	set(ZLIB_URL_MD5 16b41357b2cd81bca5e1947238e64465)
	set(ZLIB_SRC_BASE ${CMAKE_CURRENT_BINARY_DIR}/${ZLIB_PREFIX}) 

	ExternalProject_Add(${EXPAT_PREFIX}
			PREFIX ${ZLIB_PREFIX}
			URL ${ZLIB_URL}
			URL_MD5 ${ZLIB_URL_MD5}
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 0
			LOG_BUILD 0
			CONFIGURE_COMMAND ${ZLIB_SRC_BASE}/src/${EXPAT_PREFIX}/configure
				--prefix=${CMAKE_CURRENT_BINARY_DIR}
				--includedir ${CMAKE_CURRENT_BINARY_DIR}/include
				--libdir ${CMAKE_CURRENT_BINARY_DIR}/lib 
			BUILD_COMMAND make install	
	)

	add_dependencies(zlib ${EXPAT_PREFIX})
	set(ZLIB_LIBRARY ${LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

target_link_libraries(zlib INTERFACE ${ZLIB_LIBRARY})
target_include_directories(zlib INTERFACE ${ZLIB_INCLUDE_DIR})

