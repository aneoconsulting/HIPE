#!/bin/bash
cd $1
counter=0
for f in *; do
cp "$f" "eng.swiss720-exp$counter.JPG"
let counter=counter+1
done
