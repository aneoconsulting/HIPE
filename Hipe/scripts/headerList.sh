#!/bin/bash
prefix=$1
find \( -name '*.h' -o -name '*.hpp' \) -type f -exec sh -c 'echo "#include <$1/"{}">" | sed "s/\.\///g"' \;

exit 0
