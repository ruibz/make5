set(MAKE5_CLOUD /vobs/lte_oam_ucm/lte_oam/tools/make5_cloud)

INCLUDE(${MAKE5_CLOUD}/inc.cmake)
INCLUDE_DIRECTORIES(../inc)
set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

aux_source_directory(. SRCS)

add_library(base ${SRCS})

