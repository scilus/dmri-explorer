# nifti i/o library

file(GLOB NIFTI_SRC "*.c")
add_library(NIFTI_LIB ${NIFTI_SRC} ${NIFTI_HDR})
