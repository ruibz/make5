set(MAKE5_CLOUD /vobs/lte_oam_ucm/lte_oam/tools/make5_cloud)

INCLUDE(${MAKE5_CLOUD}/ut_inc.cmake)

set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
link_libraries(gtest)
link_libraries(gmock)

aux_source_directory(test TESTS)
set(GTestMain ${MAKE5_CLOUD}/gtest/gtest/gtest_main.cc)

link_libraries(base)

add_subdirectory(src)

ADD_EXECUTABLE(utRunner ${TESTS} ${GTestMain})

