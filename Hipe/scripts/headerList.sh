#!/bin/bash

find \( -name '*.h' -o -name '*.hpp' \) -type f -exec sh -c 'echo "#include <"{}">" | sed "s/\.\///g"' \;

exit 0
