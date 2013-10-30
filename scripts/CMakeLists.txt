include_directories(${QLBase_SOURCE_DIR}/code/IO
					${CFITSIO_INCLUDE_DIR} )

add_executable(fits2xml fits2xml.cpp)
target_link_libraries(fits2xml QLBase ${CFITSIO_LIBRARIES})
