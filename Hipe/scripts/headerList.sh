#!/bin/bash

find \( -name '*.h' -o -name '*.hpp' \) -type f -exec sh -c 'echo "#include <filter/Algos/"{}">" | sed "s/\.\///g"' \;

exit 0
