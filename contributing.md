# Contributing Guidelines

Of course, your contributions to the `dmri-explorer` project are very much welcomed.

## Feature Requests or Bug Reporting

Please feel free to open an issue to discuss future enhancements or to report any problem you might encounter during the installation or execution of the software. Even better, you can open a pull request to address an issue.

## Coding Standards

### `C++` code

* Do not indent namespaces.
* Const correctness: https://isocpp.org/wiki/faq/const-correctness
* Member initializer lists: https://en.cppreference.com/w/cpp/language/constructor
* Avoid setting default values for class members in headers.
* Doxygen documentation for headers. https://www.doxygen.nl/index.html
* Use most restrictive access modifier possible for class members and functions.
* Put curly braces on their own line.
* Indent with 4 whitespaces instead of tabulations.
* Avoid too long lines. Max length should be around 80 characters.
* `in` and `out` variables named with underscore or something?

### `GLSL` Shaders

* File naming convention.
* GLSL version is 4.60 : `#version 460`
* File suffix and extension :
* vertex shader `*_vert.glsl`
* fragment shader `*_frag.glsl`
* compute shader `*_comp.glsl`
* If you use a different shader type, propose a suffix following the convention.

### Third-party libraries

Third-party libraries should be included under the `deps` folder, in their own folder identified by their name. Be sure to include the license notice with the library you wish to add.
