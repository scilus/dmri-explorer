#!/bin/bash

script="$(readlink -f "${BASH_SOURCE[0]}")"
project_dir="$(dirname "$script")"

if [ -d "$project_dir/build" ] 
then
    echo "Build directory $project_dir/build already exists."
else
    echo "Creating build directory $project_dir/build."
    mkdir "$project_dir/build"
fi

cd "$project_dir/build" || exit
cmake ..
make
