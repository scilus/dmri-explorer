# RTfODFSlicer
A real-time fiber ODF slicing application for Linux using `OpenGL 4.6`.

## Installation

`Cmake` minimum version 3.11 is required for installing the application.

To build the project using `GNU Make`:
```
mkdir build
cd build
cmake ..
make
```

To run the executable:
```
./build/Engine/rtfodfslicer path/to/image.nii.gz
```

If you experience problems with the installation, make sure you have `git` installed; it is used for fetching the [`nifticlib`](https://github.com/NIFTI-Imaging/nifti_clib) library.
