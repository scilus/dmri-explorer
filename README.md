# stunning-succotash
White matter fiber ODF viewer using `OpenGL 4.6`.

## Installation

To build the project:
```
mkdir build
cd build
cmake ..
make
```

To run the executable:
```
./build/Engine/main path/to/image.nii.gz
```

## TODO
* [x] Shader program class
* [x] Program pipeline class
* [x] Indices buffer
* [x] Uniform buffers management (SSBO): `glBindBufferBase`
* [x] Struct for uniforms
* [x] Instancing with `glMultiDrawElementsIndirect`
* [ ] Includes in shaders: `glNamedStringARB`, `glCompileShaderIncludeARB`
* [ ] SSBO definition in shader include
* [x] Load data from nifti
