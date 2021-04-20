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
* [ ] Instancing with `glMultiDrawElementsIndirect`
* [ ] Includes in shaders: `glNamedStringARB`, `glCompileShaderIncludeARB`
* [ ] SSBO definition in shader include
