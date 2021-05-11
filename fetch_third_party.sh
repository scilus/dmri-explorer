#!/bin/bash
git clone https://github.com/microsoft/vcpkg.git third/vcpkg
./third/vcpkg/bootstrap-vcpkg.sh -disableMetrics
./third/vcpkg/vcpkg install glfw3 glm
