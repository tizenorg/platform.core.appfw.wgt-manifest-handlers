SET(PRJ_PATH ..)

SET(TESTS
  manifest_util_unittest
  service_handler_unittest
  content_handler_unittest
)
SET(DESTINATION_DIR manifest-parser-ut)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Executables
ADD_EXECUTABLE(manifest_util_unittest
               parser_manifest_util_unittest.cc)
ADD_EXECUTABLE(service_handler_unittest
               service_handler_unittest.cc)
ADD_EXECUTABLE(content_handler_unittest
               content_handler_unittest.cc)

INSTALL(DIRECTORY test_samples/ DESTINATION ${SHAREDIR}/${DESTINATION_DIR}/test_samples)

FOREACH(test ${TESTS})
APPLY_PKG_CONFIG(${test} PUBLIC
  LIBXML_DEPS
  PKGMGR_INFO_DEPS
  PKGMGR_PARSER_DEPS
  VCONF_DEPS
  Boost
  GTEST
  MANIFEST_PARSER_DEPS
  MANIFEST_PARSER_UTILS_DEPS
)

ENDFOREACH(test)

# FindGTest module do not sets all needed libraries in GTEST_LIBRARIES and
# GTest main libraries is still missing, so additional linking of
# GTEST_MAIN_LIBRARIES is needed.

TARGET_LINK_LIBRARIES(manifest_util_unittest PUBLIC ${TARGET_LIBNAME_PARSER}
		${TARGET_LIBNAME_WGT_MANIFEST_HANDLERS} ${GTEST_MAIN_LIBRARIES})
TARGET_LINK_LIBRARIES(service_handler_unittest PUBLIC ${TARGET_LIBNAME_PARSER}
		${TARGET_LIBNAME_WGT_MANIFEST_HANDLERS} ${GTEST_MAIN_LIBRARIES})
TARGET_LINK_LIBRARIES(content_handler_unittest PUBLIC ${TARGET_LIBNAME_PARSER}
		${TARGET_LIBNAME_WGT_MANIFEST_HANDLERS} ${GTEST_MAIN_LIBRARIES})

FOREACH(test ${TESTS})
  INSTALL(TARGETS ${test} DESTINATION ${BINDIR}/${DESTINATION_DIR})
ENDFOREACH(test)
