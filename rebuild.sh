#!/bin/bash

# Function to handle errors
handle_error() {
    echo -e "\nError: $1\n"
    exit 1
}

# Ensure clean.sh is executable
echo -e "\nEnsuring clean.sh is executable...\n"
chmod +x ./clean.sh
if [ $? -ne 0 ]; then
    handle_error "Failed to make clean.sh executable."
fi

# Run the clean.sh script to clean the workspace
echo -e "\nCleaning the workspace...\n"
./clean.sh
if [ $? -ne 0 ]; then
    handle_error "Failed to clean the workspace."
fi

# Ensure build.sh is executable
echo -e "\nEnsuring build.sh is executable...\n"
chmod +x ./build.sh
if [ $? -ne 0 ]; then
    handle_error "Failed to make build.sh executable."
fi

# Run the build.sh script to build the workspace
echo -e "\nBuilding the workspace...\n"
./build.sh
if [ $? -ne 0 ]; then
    handle_error "Failed to build the workspace."
fi

echo -e "\nWorkspace rebuild completed successfully.\n"