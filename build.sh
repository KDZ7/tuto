#!/bin/bash

# Function to handle errors
handle_error() {
    echo -e "\nError: $1\n"
    exit 1
}

# Build the workspace using colcon
echo -e "\nBuilding the workspace...\n"
colcon build --symlink-install 
if [ $? -ne 0 ]; then
    handle_error "Failed to build the workspace."
fi

echo -e "\nWorkspace build completed successfully.\n"
