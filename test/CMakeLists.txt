CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#PROJECT(privacy-manager-test)

SET(CMAKE_INSTALL_PREFIX /usr)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")
SET(VERSION 0.0)

SET(VERSION_MAJOR 0)
SET(VERSION "${VERSION_MAJOR}.0.1")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED dlog)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS_PROFILING    " -g -pg")
SET(CMAKE_CXX_FLAGS_PROFILING  " -std=c++0x -g -pg")
SET(CMAKE_C_FLAGS_DEBUG        " -g")
SET(CMAKE_CXX_FLAGS_DEBUG      " -std=c++0x -g")
SET(CMAKE_C_FLAGS_RELEASE      " -g")
SET(CMAKE_CXX_FLAGS_RELEASE    " -std=c++0x -g")
SET(CMAKE_C_FLAGS_CCOV         " -g --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       " -std=c++0x -g --coverage")

SET(test_src_dir "${CMAKE_SOURCE_DIR}/test/")
SET(test_include_dir "${CMAKE_SOURCE_DIR}/test/")
SET(client_include_dir "${CMAKE_SOURCE_DIR}/client/inc/")
SET(common_include_dir "${CMAKE_SOURCE_DIR}/common/inc/")

## Additional flag
ADD_DEFINITIONS("-fvisibility=hidden")
ADD_DEFINITIONS("-Wall -Werror")
ADD_DEFINITIONS("-DDLOG_ERROR_ENABLED")

###################################################################################################
## for privacy-manager-test (executable)
INCLUDE_DIRECTORIES(${pkgs_INCLUDE_DIRS} ${test_include_dir} ${client_include_dir} ${common_include_dir})
SET(PRIVACY_MANAGER_TEST_SOURCES 
	${test_src_dir}/TestMain.cpp
	)

SET(PRIVACY_MANAGER_TEST_LDFLAGS " -module -avoid-version -L../client")
SET(PRIVACY_MANAGER_TEST_CFLAGS  " ${CFLAGS} -fPIC ")
#SET(PRIVACY_MANAGER_TEST_LIBADD " ")

ADD_EXECUTABLE(privacy-manager-test ${PRIVACY_MANAGER_TEST_SOURCES})
ADD_DEPENDENCIES(privacy-manager-test privacy-manager-client)

TARGET_LINK_LIBRARIES(privacy-manager-test ${pkgs_LDFLAGS} ${pkgs_LIBRARIES} privacy-manager-client)
SET_TARGET_PROPERTIES(privacy-manager-test PROPERTIES COMPILE_FLAGS "${PRIVACY_MANAGER_TEST_CFLAGS}")
#SET_TARGET_PROPERTIES(privacy-manager-test PROPERTIES SOVERSION ${VERSION_MAJOR})
#SET_TARGET_PROPERTIES(privacy-manager-test PROPERTIES VERSION ${VERSION})
###################################################################################################

INSTALL(TARGETS privacy-manager-test DESTINATION ../bin COMPONENT RuntimeLibraries)
#INSTALL(FILES ${PRIVACY_MANAGER_TEST_HEADERS} DESTINATION ../include)