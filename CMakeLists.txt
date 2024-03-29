
cmake_minimum_required(VERSION 3.9)
project(peptideUtils VERSION 2.1.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

set(SHARE_DIR "${PROJECT_SOURCE_DIR}/share" CACHE STRING "Path to directory with defaultResidueAtoms.txt")

#TODO Make config.h.in file. Right now I don't want to make the master and cmake branches incompatible.
add_compile_definitions(SHARE_DIR=\"${SHARE_DIR}\")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/thirdparty")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_BUILD_TYPE Debug)

add_library(peptideUtils STATIC
        src/utils.cpp
        src/tsvFile.cpp
        src/thirdparty/msnumpress/MSNumpress.cpp
        src/msInterface/internal/base64_utils.cpp
        src/msInterface/internal/xml_utils.cpp
        src/msInterface/msScan.cpp
        src/msInterface/msInterface.cpp
        src/msInterface/ms2File.cpp
        src/msInterface/mzMLFile.cpp
        src/msInterface/mzXMLFile.cpp
        src/fastaFile.cpp
        src/bufferFile.cpp
        src/molecularFormula.cpp
        src/sequenceUtils.cpp)

target_include_directories(peptideUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(EXCLUDE_FROM_DOXYGEN ${CMAKE_CURRENT_SOURCE_DIR}/include/thirdparty)
set_target_properties(peptideUtils PROPERTIES PUBLIC_HEADER "include/sequenceUtils.hpp;include/molecularFormula.hpp;include/fastaFile.hpp;include/bufferFile.hpp;include/msInterface/mzXMLFile.hpp;include/msInterface/msInterface.hpp;include/msInterface/mzMLFile.hpp;include/msInterface/internal/xml_utils.hpp;include/msInterface/internal/base64_utils.hpp;include/msInterface/msScan.hpp;include/msInterface/ms2File.hpp;include/exceptions.hpp;include/utils.hpp;include/tsvFile.hpp;include/thirdparty/msnumpress/MSNumpress.hpp;include/thirdparty/rapidxml/rapidxml_iterators.hpp;include/thirdparty/rapidxml/rapidxml_print.hpp;include/thirdparty/rapidxml/rapidxml_utils.hpp;include/thirdparty/rapidxml/rapidxml.hpp")

option(SYSTEM_ZLIB "Use system zlib library" ON)
option(ENABLE_ZLIB "Add support for zlib decompression" ON)
if(ENABLE_ZLIB)
	if(NOT ZLIB_FOUND)
		set(ZLIB_VERSION "1.2.11")
		include(zlib)
		add_dependencies(peptideUtils zlib)
	else()
        message("-- ZLIB already found")
	endif()

    message("-- peptideUtils zlib shared lib ${ZLIB_LIBRARIES}")
    message("-- peptideUtils zlib include dir ${ZLIB_INCLUDE_DIRS}")
	target_link_libraries(peptideUtils ${ZLIB_LIBRARIES})
	target_include_directories(peptideUtils PUBLIC ${ZLIB_INCLUDE_DIRS})
	add_compile_definitions("ENABLE_ZLIB")
endif()

install(TARGETS peptideUtils
		LIBRARY DESTINATION lib
		PUBLIC_HEADER DESTINATION include/peptideUtils
		)

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" OFF)
if(BUILD_DOC MATCHES ON)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif()

#build test file (for debugging purposes)
option(BUILD_TEST "Build test executable" OFF)
if(BUILD_TEST MATCHES ON)
    add_executable(test test/main.cpp)
    target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(test zlib peptideUtils)
endif()

