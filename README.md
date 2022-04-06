![Banner logo](banner-logo.png)
# dmri-explorer : A Diffusion MRI Visualization Application
A real-time diffusion MRI slicing application for Linux using `OpenGL 4.6`. It can visualize spherical functions expressed as a series of spherical harmonics coefficients in DIPY's `descoteaux07` legacy real symmetric or full SH basis (See [this link](https://dipy.org/documentation/1.4.1./theory/sh_basis/) for more details.).


## Installation

To start using `dmri-explorer`, clone this repository.

### Requirements

Before installing the software, please make sure you meet the requirements below.

* `Cmake` minimum version 3.11 is required for installing the application.
* The generator used by default is `GNU Make`.
* A compiler supporting `C++17` features is required (for GCC, `C++17` is available for [versions 5 and above](https://gcc.gnu.org/projects/cxx-status.html#cxx17)).
* Make sure you have `git` installed; it is used for fetching the [`nifticlib` library](https://github.com/NIFTI-Imaging/nifti_clib).
* You have an OpenGL implementation available on your system. On Ubuntu, it can be installed with `sudo apt install mesa-common-dev`. `mesa-common-dev` is an open-source implementation of OpenGL. See this [link from wikipedia](https://en.wikipedia.org/wiki/OpenGL#Implementations) for more information.


### Building the application

The build the program, run the following commands from the project root directory:
```
mkdir build
cd build
cmake ..
make
```

Alternatively, a helper script containing the four commands shown above is made available for building the software. It can be launched using: 
```
./install.sh
```
The above script creates the build directory, runs `Cmake` and `make`. The executable file will be in the folder `${project_root}/build/Engine`.


### Adding to `path`
Once the program is built, you can add the path to the directory containing the executable to your `.bashrc` configuration to make it available system-wide.

```
export PATH='${absolute_path_to_project}/build/Engine':$PATH
```

## Running the application
The application can be launched using the command:
```
dmriexplorer path/to/image.nii.gz
```
When working with a big image, you may encounter a "Window not responding" message at application startup. Don't worry, it will go away once the image is copied on the GPU.

To display all available command line arguments, use the flag `--help`.

## How to cite

The software has been presented as part of the CDMRI'21 workshop. It has however evolved since then and will keep getting better over time. For the performances reported at the workshop, please refer to the release [v0.1](https://github.com/scilus/dmri-explorer/releases/tag/v0.1). However, **we highly encourage the user to use the latest version of the application** on the `main` branch, as efforts have been made to lower GPU and CPU memory usage.

**If you enjoy this application, please consider citing the following work:**

*Poirier C., Descoteaux M., Gilet G. (2021) Accelerating Geometry-Based Spherical Harmonics Glyphs Rendering for dMRI Using Modern OpenGL. In: Cetin-Karayumak S. et al. (eds) Computational Diffusion MRI. CDMRI 2021. Lecture Notes in Computer Science, vol 13006. Springer, Cham. https://doi.org/10.1007/978-3-030-87615-9_13*


## Contributing
Contributions are welcome and encouraged:
* If you discover bugs (installation problems, in-app bugs, etc.), please [open an issue](https://github.com/scilus/dmri-explorer/issues) describing the error and the steps to reproduce it;
* If you have a feature request, you are also encouraged to open an issue describing your need;
* Pull-requests are of course welcome! Please refer to the [contributing guidelines](https://github.com/scilus/dmri-explorer/blob/main/contributing.md).
