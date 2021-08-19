# RT fODF Slicer
A real-time fiber ODF slicing application for Linux using `OpenGL 4.6`.

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
