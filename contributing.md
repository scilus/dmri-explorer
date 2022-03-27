# Contributing Guidelines

Your contributions to the `dmri-explorer` project are very much welcomed!

## Feature Requests or Bug Reporting

Please feel free to open an issue to discuss future enhancements or to report any problem you might encounter during the installation or execution of the software. Even better, you can open a pull request to address an issue.

## Coding Standards
The source code is written in `C++` and OpenGL shading language (`glsl`).

### `C++` code
__Indentation and line length__

The code is indented with 4 space characters instead of tabs. Use indentation everywhere except for namespaces. Also avoid too long lines. Maximum line length should be around 80 characters. If possible, split excessively long lines on multiple lines.

__Const correctness__

The code should be `const`-correct, that is _**every variable that will not change during its lifetime should be marked as const.**_ Refer to [this link](https://isocpp.org/wiki/faq/const-correctness) for more details on const correctness.

__Class members__

Classes should be defined in header files. Instead of setting default values in the header files, do so in the constructor. Whenever possible, initialize member variables in [member initializer lists](https://en.cppreference.com/w/cpp/language/constructor).

__Class access modifiers__

Always use the most restrictive access modifier possible for class members and functions. Always prefer getters and setters to `friend` classes, or public class members.

__Curly braces__

Always put curly braces on their own line. Always use curly braces, even for oneline `if` and `for` statements.

__Pointers and reference__

When declaring pointers (`*`) and reference (`&`) arguments, put the appropriate symbol right next to the type. For example, prefer `const type* myArg` to `const type *myArg`.

__Documentation__

Please write documentation in header files using the [doxygen](https://www.doxygen.nl/index.html) standard. A `doxyfile` is available for generating the doxygen documentation locally.


### `GLSL` Shaders

__Shader file naming convention__

OpenGL shaders are written in the OpenGL shading language (`glsl`) version 4.60. Shaders are stored in the `Engine/shaders/` directory and should follow the naming convention `{name}_{type}.glsl`. For example:
* `*_frag.glsl` for fragment shaders;
* `*_vert.glsl` for vertex shaders;
* `*_comp.glsl` for compute shaders.

If you use a different shader type, please propose a suffix following this convention.

__Variable name style__

In shader code, we distinguish between variables shared between shaders (`in` and `out` keywords) and local variables by using a different style:
* Use `snake_case` for `in` and `out` variables;
* Use `camelCase` for local variables.


### Third-party libraries

Third-party libraries should be included under the `deps` folder, in their own folder identified by their name. Make sure to include the license notice with the library you wish to add.
