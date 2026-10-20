# Install and build

ark requires CMake 3.21+, Ninja, a C++20 compiler and the dependencies selected by its vcpkg manifest. Use Visual Studio 2022 Developer PowerShell on Windows, or a compatible Clang/GCC toolchain on macOS/Linux.

<Steps>
<Step title="Clone and initialize dependencies">

```sh
git clone https://github.com/OpenArkStudio/ark.git
cd ark
git submodule update --init --recursive
```

</Step>
<Step title="Configure the application build">

Install vcpkg and set the `VCPKG_ROOT` environment variable to that checkout. Select Boost for the network dependency.

On macOS/Linux, use a POSIX shell. The [macOS compiler and SDK example](#macos-compiler-and-sdk) below selects the Apple toolchain explicitly.

```sh
cmake -S . -B app-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES="application;boost" \
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
```

On Windows, use Visual Studio 2022 Developer PowerShell:

```powershell
cmake -S . -B app-build -G Ninja `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  "-DVCPKG_MANIFEST_FEATURES=application;boost" `
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
```

</Step>
<Step title="Build">

```sh
cmake --build app-build --parallel 2
```

Multi-configuration generators also need `--config Release`. This command writes executables, plugins and static archives to `app-build/bin/`. Use a separate CMake directory for each Release, Debug or sanitizer build. The [quick start](quick-start.md) shows how to copy the selected binaries into a runtime directory. The [build options](../reference/build-options.md) control applications, examples and tests.

</Step>
</Steps>

For a smaller correctness build, follow [network regression tests](../networking.md#build-and-run-the-network-tests). After preparing a separate runtime, use the [launch helpers](../guides/applications.md#run-one-application) under `deployment/scripts/` with its path explicitly selected.

## macOS compiler and SDK

Select the C compiler, C++ compiler and SDK from the same active Xcode or Command Line Tools installation with `xcrun`. When changing the compiler or SDK, use a new build directory so CMake does not retain the previous selection. Run the following commands instead of the configure and build commands above:

```sh
ark_macos_sdk="$(xcrun --sdk macosx --show-sdk-path)"
cmake -S . -B app-build-macos -G Ninja \
  -DCMAKE_C_COMPILER="$(xcrun --sdk macosx --find clang)" \
  -DCMAKE_CXX_COMPILER="$(xcrun --sdk macosx --find clang++)" \
  -DCMAKE_OSX_SYSROOT="$ark_macos_sdk" \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES="application;boost" \
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build app-build-macos --parallel 2
```

Use `app-build-macos` in place of `app-build` in the subsequent runtime-copy paths.

<Aside type="caution" title="Homebrew LLVM 22.1.7 with macOS SDK 27.0">

This compiler/SDK combination has a standard-header compatibility issue that depends on include order and can report undeclared `INFINITY` or `DBL_EPSILON`. If these errors occur, use the Apple Clang and SDK selection above in a new build directory.

</Aside>

## Upgrade arknet

The repository's `ARKNET_REVISION` default selects a fixed arknet commit when CMake fetches the library. Updating ark's source does not replace an `ARKNET_REVISION` value already saved in an existing `CMakeCache.txt`.

Use a new build directory and repeat the configure command above to take the repository default. To retain an existing build directory, replace `RELEASE_COMMIT` with the full 40-character commit of the selected [arknet release](https://github.com/OpenArkStudio/arknet/releases), then explicitly update the cached value:

```sh
cmake -S . -B app-build -DARKNET_REVISION=RELEASE_COMMIT
cmake --build app-build --parallel 2
```

The existing build directory retains its other configured options. Confirm `ARKNET_REVISION` in `app-build/CMakeCache.txt`; multi-configuration generators still require `--config Release` when building.

A nonempty `ARKNET_SOURCE_DIR` uses that local checkout instead of fetching `ARKNET_REVISION`. Update the checkout explicitly for development. To return to the pinned fetch, configure with `-DARKNET_SOURCE_DIR=` and the desired `-DARKNET_REVISION=RELEASE_COMMIT`. These remain optional CMake overrides; the CI policy compares the repository defaults, not a developer's cache or local checkout.
