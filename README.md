# stunning-succotash
OpenGL viewer.

## Installation
To install third party libraries, run the command below.
```
./fetch_third_party.sh
```

To build the project:
```
mkdir build
cd build
cmake ..
make
```

To run the executable:
```
cd build/Engine
./main
```

## TODO
* [x] Shader program class
* [x] Program pipeline class
* [x] Indices buffer
* [x] Uniform buffers management (SSBO): `glBindBufferBase`
* [x] Struct for uniforms
* [-] Instancing with `glMultiDrawElementsIndirect`
* [ ] Includes in shaders: `glNamedStringARB`, `glCompileShaderIncludeARB`
* [ ] SSBO definition in shader include
* [ ] Load data from nifti

## On `glMultiDrawElementsIndirect` errors
GL_INVALID_OPERATION is generated if no buffer is bound to the GL_ELEMENT_ARRAY_BUFFER binding, or if such a buffer's data store is currently mapped.

GL_INVALID_OPERATION is generated if a non-zero buffer object name is bound to an enabled array or to the GL_DRAW_INDIRECT_BUFFER binding and the buffer object's data store is currently mapped. 