# Binary

Build artifacts are written to `<cmake-build-directory>/bin`. Use separate
build directories for Release, Debug and sanitizer builds so their archives,
plugins and executables cannot overwrite each other:

```sh
cmake -S . -B out/release -DCMAKE_BUILD_TYPE=Release
cmake --build out/release --parallel 2
```

This build produces `out/release/bin/app` (`app.exe` on Windows) and the enabled
plugins in the same directory. `cmake -S . -B build` retains the `build/bin`
location. The Docker build copies its artifacts from `cmake-build/bin`.

When staging a server, copy the chosen build's artifacts into the runtime
directory's `bin/` alongside its configuration and resources. The scripts below
use this runtime layout; they do not select a CMake build directory.

This folder is the runtime folder, include

- bin
- binlog
- bus_conf
- plugin_conf
- resource
- tools

## Batch/shell

| name     | descriptor                            |
| -------- | ------------------------------------- |
| rund.bat | run debug version in Windows          |
| run.bat  | run release version in Windows        |
| kill.bat | kill debug & release version in Linux |
|          |                                       |
| run.sh   | run release version in Linux          |
| rund.sh  | run debug version in Linux            |
| kill.sh  | kill debug & release version in Linux |
