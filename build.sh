#!/bin/bash

echo "Building project..."

debug=${1:-0}

echo "Running make command..."
make all DEBUG=$debug
echo "Installing binaries..."
sudo make install DEBUG=$debug
