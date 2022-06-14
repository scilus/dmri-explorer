# nifti i/o library
add_compile_definitions(HAVE_ZLIB)

file(GLOB NIFTI_SRC "src/*.c")
add_library(NIFTI_LIB ${NIFTI_SRC})

find_package(ZLIB REQUIRED)
target_link_libraries(NIFTI_LIB PRIVATE ZLIB::ZLIB)

include_directories("include/")
