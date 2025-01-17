#!/bin/bash

# Script to clean the ROS 2 workspace
# This script removes the build, install, and log directories from both the root directory and the 'src/' directory.

# Function to handle errors
# Arguments:
#   $1 - Error message to display
handle_error() {
    echo -e "\nError: $1\n"
    exit 1
}

# Remove root-level directories
# This includes 'build', 'install', and 'log' directories located at the root of the workspace.
echo -e "\nRemoving 'build', 'install', and 'log' directories from the root directory...\n"
rm -rf build install log
if [ $? -ne 0 ]; then
    handle_error "Failed to remove 'build', 'install', or 'log' directories from the root directory."
fi

# Remove directories within 'src/'
# This includes 'build', 'install', and 'log' directories specifically located inside the 'src/' directory.
echo -e "\nRemoving 'build', 'install', and 'log' directories from the 'src/' directory...\n"
rm -rf src/build src/install src/log
if [ $? -ne 0 ]; then
    handle_error "Failed to remove 'src/build', 'src/install', or 'src/log' directories."
fi

# Remove other files specific extensions
echo -e "\nRemoving other files specific extensions...\n"
rm -rf *pdf *.gv *.out

echo -e "\nWorkspace has been successfully cleaned.\n"
