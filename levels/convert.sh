#!/bin/bash

mkdir -p "./sd-card/levels"

counter=1

for png_file in ./png/*.png; do
    filename=$(printf "%02d.bmp" $counter)

    magick "$png_file" -type truecolor "../sd-card/levels/$filename"

    ((counter++))
done

echo "Conversion complete! Processed $((counter-1)) files."