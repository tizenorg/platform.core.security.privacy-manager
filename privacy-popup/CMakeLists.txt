INCLUDE(FindPkgConfig)
pkg_check_modules(privacy-popup-ug-test_pkgs
#pkg_check_modules(privacy-popup_pkgs
    REQUIRED
    elementary
    evas
    ecore-x
    ui-gadget-1
    dlog
    capi-appfw-application
	appcore-efl
)
SET (this_target tizenprv00.privacy-popup)
#SET (this_target privacy-manager-privacy-popup)

SET(SRCS
    privacy-popup.c
)

#SET(INCLUDEDIR "\${prefix}/include")
#INCLUDE_DIRECTORIES(${privacy-popup_pkgs_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${privacy-popup-ug-test_pkgs_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc)
INCLUDE_DIRECTORIES(${ug-privacy-popup-ug-efl_pkgs_INCLUDE_DIRS} /usr/include/privacy_manager)


FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")

IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
    MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

MESSAGE("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

ADD_DEFINITIONS("-DSLP_DEBUG")

#SET(CMAKE_CXX_CFLAGS "${CMAKE_CXX_FLAGS} -fPIE")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")


ADD_EXECUTABLE(${this_target} ${SRCS})

#TARGET_LINK_LIBRARIES(${this_target} --as-needed -pie)
#TARGET_LINK_LIBRARIES(${this_target} ${privacy-popup_pkgs_LDFLAGS} ${privacy-popup_pkgs_LIBRARIES})
TARGET_LINK_LIBRARIES(${this_target} "-lpkgmgr-info")
TARGET_LINK_LIBRARIES(${this_target} "-lprivacy-manager-client")
TARGET_LINK_LIBRARIES(${this_target} ${privacy-popup-ug-test_pkgs_LDFLAGS} ${privacy-popup-ug-test_pkgs_LIBRARIES})


TARGET_LINK_LIBRARIES(${this_target}
	#${privacy-popup_LIBRARIES}
	${privacy-popup-ug-test_LIBRARIES}
)


INSTALL(TARGETS
	${this_target}
    DESTINATION
    /usr/bin/
)
#INSTALL(FILES ${CMAKE_SOURCE_DIR}/packaging/tizenprv00.privacy-popup.rule DESTINATION /etc/smack/accesses2.d)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/packaging/${this_target}.xml DESTINATION /usr/share/packages)
