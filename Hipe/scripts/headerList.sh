#!/bin/bash
prefix=$1
find \( -name '*.h' -o -name '*.hpp' \) -type f -exec sh -c "echo \"#include <${prefix}/"{}">\" | sed \"s/\.\///g\"" \;

exit 0
