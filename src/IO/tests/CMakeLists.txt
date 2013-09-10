find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

include_directories(${QLBase_SOURCE_DIR}/src
                    ${Boost_INCLUDE_DIRS}
                    ${CFITSIO_INCLUDE_DIR}
                    )

add_executable(testFileFITS testFileFITS.cpp)
target_link_libraries(testFileFITS
                      QLBase.0.1
                      ${CFITSIO_LIBRARIES}
                      ${Boost_LIBRARIES}
                      )

add_executable(testFileText testFileText.cpp)
target_link_libraries(testFileText
                      QLBase.0.1
                      ${CFITSIO_LIBRARIES}
                      ${Boost_LIBRARIES}
                      )

set(TESTFILES img.csv sample.fits sample.txt)
foreach(testfile ${TESTFILES})
add_custom_command(TARGET testFileFITS
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   ${QLBase_SOURCE_DIR}/src/IO/tests/${testfile}
                   ${CMAKE_CURRENT_BINARY_DIR}/${testfile})
                   set(RES ${RES} ${CMAKE_CURRENT_BINARY_DIR}/${testfile})
endforeach(testfile)
add_custom_command(SOURCE testFileFITS TARGET testFileFITS DEPENDS ${res})

add_custom_command(TARGET testFileFITS POST_BUILD COMMAND testFileFITS)
add_custom_command(TARGET testFileFITS POST_BUILD COMMAND testFileText)
