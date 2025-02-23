#!/bin/bash

# Check if ImageMagick is installed
if ! command -v convert &> /dev/null; then
  echo "Error: ImageMagick is not installed. Please install it to use this script."
  exit 1
fi

# Ensure chordcat.png exists
if [ ! -f "chordcat.png" ]; then
  echo "Error: chordcat.png not found in the current directory."
  exit 1
fi

# Loop over directories that match the resolution pattern (e.g., 128x128)
for dir in *x*; do
  if [ -d "$dir" ]; then
    echo "Converting chordcat.png to resolution $dir..."
    convert chordcat.png -resize "$dir" "$dir/chordcat.png"
  fi
done

echo "All conversions complete."
