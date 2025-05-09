![Banner logo](banner-logo.png)
# dmri-explorer : A Diffusion MRI Visualization Application
A real-time diffusion MRI slicing application for Linux and Windows using `OpenGL 4.6`. It can visualize tensor and multi-tensor images in MRtrix, DIPY and FSL format as well as spherical functions expressed as a series of spherical harmonics coefficients in DIPY's `descoteaux07` legacy real symmetric or full SH basis (See [this link](https://dipy.org/documentation/1.4.1./theory/sh_basis/) for more details.). The application also supports grayscale and color background images. Several colormaps are available for tensor visualisation.


## Installation

To start using `dmri-explorer`, clone this repository.

### Requirements

Before installing the software, please make sure you meet the requirements below.

* `Cmake` minimum version 3.11 is required for installing the application.
* A compiler supporting `C++17` features is required (for GCC, `C++17` is available for [versions 5 and above](https://gcc.gnu.org/projects/cxx-status.html#cxx17)).
* You have an OpenGL implementation available on your system. On Ubuntu, it can be installed with `sudo apt install mesa-common-dev`. `mesa-common-dev` is an open-source implementation of OpenGL. See this [link from wikipedia](https://en.wikipedia.org/wiki/OpenGL#Implementations) for more information.
* **If you are using Linux**, please note that the generator used by default is `GNU Make`.
* **If you are a Windows user**, the expected generator is `Visual Studio 2019` and the application must be built for `x64` architecture. You can get the latest MSVC compiler toolset [here](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022).
* The application is tested with NVIDIA GPU and is not garanteed to work on other hardware. In particular, **the application does not run on Intel integrated graphics card (CPU) on Windows**.

### Building the application
The application is tested on both **Ubuntu 20.04** and **Windows 10**.

#### On Linux
To build the program, run the following commands from the project root directory:
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

##### Troubleshooting
Libraries `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev` may be missing when generating the CMake project. These can be installed by running:
```
sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

#### On Windows
The Windows build is also done using `cmake`. The build has been tested with `msbuild` version 16.9. Using the **Developer Command Prompt for VS**, the program can be built with:
```
cd /path/to/dmriexplorer/
mkdir build
cd build
cmake ..
msbuild dmriexplorer.sln
```

Alternatively, for Visual Studio users, the file `dmriexplorer.sln` generated by `cmake` can be opened with Visual Studio and built through the interface.

### Adding to `path`
Once the program is built, you can add the path to the directory containing the executable to your `.bashrc` configuration to make it available system-wide. On Linux, this is done with

```
export PATH='${absolute_path_to_project}/build/Engine':$PATH
```

On Windows, this can be done manually from the `Control panel`.

## Usage
The application can be launched using the command:

```
dmriexplorer [options] image
```

### Spherical Functions
For visualizing spherical function images use the `-f` option

Example:
```
dmriexplorer -f path/to/odf.nii.gz
```

### Tensors
Ir order to visualize tensor images use the `-t` option

Example:
```
dmriexplorer -t path/to/tensor.nii.gz
```

####  Multi-Tensors
Several tensor layers can be added for visualizing a multi-tensor image. Make sure that all layers have the same tensor format.

Example:
```
dmriexplorer -t path/to/tensor_layer1.nii.gz -t path/to/tensor_layer2.nii.gz -t path/to/tensor_layer3.nii.gz
```

The default tensor formating follows the MRtrix format ([link here](https://mrtrix.readthedocs.io/en/dev/reference/commands/dwi2tensor.html)).
This behaviour can be altered using the `-o` option. All the supported formats are: `mrtrix`, `dipy` and `fsl`.

When working with a big image or several layers, you may encounter a "Window not responding" message at application startup. Don't worry, it will go away once the image is copied on the GPU.

To display all available command line arguments, use the flag `--help`.

## How to cite

The software has been presented as part of the CDMRI'21 workshop and the ISMRM'22 diffusion workshop. It has however evolved since then and will keep getting better over time. For the performances reported at the CDMRI workshop, please refer to the release [v0.1](https://github.com/scilus/dmri-explorer/releases/tag/v0.1). However, **we highly encourage the user to use the latest version of the application** on the `main` branch, as efforts have been made to lower GPU and CPU memory usage.

**If you enjoy this application, please consider citing the following work:**

* Original SH visualisation method:
    >Poirier C., Descoteaux M., Gilet G. (2021) Accelerating Geometry-Based Spherical Harmonics Glyphs Rendering for dMRI Using Modern OpenGL. In: Cetin-Karayumak S. et al. (eds) Computational Diffusion MRI. CDMRI 2021. Lecture Notes in Computer Science, vol 13006. Springer, Cham. https://doi.org/10.1007/978-3-030-87615-9_13

* Multi-tensor support:
    >Hernandez-Gutierrez E., Poirier C., Coronado-Leija R., Descoteaux M. (2022) Real-Time Rendering of Massive Multi-Tensor Fields Using Modern OpenGL. Proceedings of the International Society for Magnetic Resonance in Medicine.

## Contributing
Contributions are welcome and encouraged:
* If you discover bugs (installation problems, in-app bugs, etc.), please [open an issue](https://github.com/scilus/dmri-explorer/issues) describing the error and the steps to reproduce it;
* If you have a feature request, you are also encouraged to open an issue describing your need;
* Pull-requests are of course welcome! Please refer to the [contributing guidelines](https://github.com/scilus/dmri-explorer/blob/main/contributing.md).
