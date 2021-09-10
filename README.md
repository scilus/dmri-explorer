# RT fODF Slicer
A real-time fiber ODF slicing application for Linux using `OpenGL 4.6`. It can visualize spherical functions expressed as a series of spherical harmonics coefficients in DIPY's `descoteaux07` legacy real symmetric SH basis (See [this link](https://dipy.org/documentation/1.4.1./theory/sh_basis/) for more details.).

## CDMRI'21
The software is presented as part of the CDMRI'21 workshop. It has however evolved since then and will keep getting better over time. For the implementation presented at the workshop, please refer to the release [v0.1](https://github.com/CHrlS98/RTfODFSlicer/releases/tag/v0.1).

## Installation

`Cmake` minimum version 3.11 is required for installing the application. The generator used by default is `GNU Make`. Make sure you have `git` installed; it is used for fetching the [`nifticlib` library](https://github.com/NIFTI-Imaging/nifti_clib).


### Quick installation
The program can be installed by running:
```
./install.sh
```
The above script creates the build directory, runs `Cmake` and `make` and add the executable path the system `PATH`.

### Step-by-step installation
Alternatively, the program can be built by running:
```
mkdir build
cd build
cmake ..
make
```

The executable file will be in the folder `${project_root}/build/Engine`.

Then you can add the path to the directory containing the executable to your `.bashrc` configuration to make it available system-wide.

```
export PATH='${absolute_path_to_project}/build/Engine':$PATH
```

## Running the application
To run the executable:
```
rtfodfslicer path/to/image.nii.gz
```

## Documentation
Documentation is available on the application's [website](https://chrls98.github.io/RTfODFSlicer/html/index.html). It contains up-to-date documentation on the code generated with Doxygen.

## Test for documentation update
Test for documentation update. RETEST!!