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

if [ -e "$project_dir/.cache-install" ] 
then
    echo "Skipping path update. Delete .cache-install file and rerun to update."
else
    echo "Updating path..."
    echo "export PATH='$project_dir/build/Engine':\$PATH" >> ~/.bashrc
    source ~/.bashrc
    touch "$project_dir/.cache-install"
fi
echo "Done!"
