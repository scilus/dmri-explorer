#!/bin/bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh -disableMetrics
vcpkg/vcpkg install glfw3 glad glm
