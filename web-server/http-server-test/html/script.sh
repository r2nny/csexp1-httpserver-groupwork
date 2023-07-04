#!/bin/bash

i=0
for file in *.jpg; do
  new_name=$(printf "%03d.jpg" "$i")
  mv "$file" "$new_name"
  ((i++))
done
