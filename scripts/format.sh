#!/bin/bash
echo -n "Running dos2unix     "
find ../src -name "*\.h" -o -name "*\.hpp" -o -name "*\.cpp"|grep -v bundled|xargs -I {} sh -c "dos2unix '{}' 2>/dev/null; echo -n '.'"
echo
echo -n "Running clang-format "
find ../src -name "*\.h" -o -name "*\.hpp" -o -name "*\.cpp"|grep -v bundled|xargs -I {} sh -c "clang-format -i -style=file {}; echo -n '.'"
echo


