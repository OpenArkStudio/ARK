# 安装与构建

ark 需要 CMake 3.21+、Ninja、支持 C++20 的编译器，以及 vcpkg manifest 选定的依赖。Windows 使用 Visual Studio 2022 Developer PowerShell，macOS/Linux 使用兼容的 Clang 或 GCC 工具链。

<Steps>
<Step title="克隆仓库">

```sh
git clone https://github.com/OpenArkStudio/ark.git
cd ark
```

</Step>
<Step title="配置应用构建">

安装 vcpkg，将环境变量 `VCPKG_ROOT` 指向对应目录。网络依赖选择 Boost。

macOS/Linux 使用 POSIX shell。下方的 [macOS 编译器与 SDK 示例](#macos-编译器与-sdk)显式选择 Apple 工具链。

```sh
cmake -S . -B app-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES="application;boost;etcd-client" \
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
```

Windows 使用 Visual Studio 2022 Developer PowerShell：

```powershell
cmake -S . -B app-build -G Ninja `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  "-DVCPKG_MANIFEST_FEATURES=application;boost;etcd-client" `
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
```

</Step>
<Step title="构建">

```sh
cmake --build app-build --parallel 2
```

使用多配置生成器时还需添加 `--config Release`。该命令将可执行文件、插件和静态库输出到 `app-build/bin/`。Release、Debug 和 sanitizer 构建分别使用独立的 CMake 目录。按[快速开始](quick-start_CN.md)将选定的二进制文件复制到运行目录。[构建选项](../reference/build-options_CN.md)可控制应用、示例和测试。

</Step>
</Steps>

只验证网络正确性时，按[网络回归测试](../networking_CN.md)进行精简构建。运行 `build/` 下的脚本前，先核对应用与部署配置。

## macOS 编译器与 SDK

通过 `xcrun` 选择当前 Xcode 或 Command Line Tools 中配套的 C 编译器、C++ 编译器和 SDK。更换编译器或 SDK 时，使用新的构建目录，避免 CMake 保留原有选择。用以下命令替代上面的配置与构建命令：

```sh
ark_macos_sdk="$(xcrun --sdk macosx --show-sdk-path)"
cmake -S . -B app-build-macos -G Ninja \
  -DCMAKE_C_COMPILER="$(xcrun --sdk macosx --find clang)" \
  -DCMAKE_CXX_COMPILER="$(xcrun --sdk macosx --find clang++)" \
  -DCMAKE_OSX_SYSROOT="$ark_macos_sdk" \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES="application;boost;etcd-client" \
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build app-build-macos --parallel 2
```

后续复制运行文件时，将示例路径中的 `app-build` 替换为 `app-build-macos`。

<Aside type="caution" title="Homebrew LLVM 22.1.7 与 macOS SDK 27.0">

这组编译器与 SDK 存在与包含顺序有关的标准头文件兼容问题，可能报 `INFINITY` 或 `DBL_EPSILON` 未声明。遇到这些错误时，按上面的命令选择配套的 Apple Clang 和 SDK，并使用新的构建目录。

</Aside>

## 升级 arknet

由 CMake 获取网络库时，仓库中的 `ARKNET_REVISION` 默认值指定一个固定的 arknet commit。更新 ark 源码不会替换已有 `CMakeCache.txt` 中保存的 `ARKNET_REVISION`。

使用新的构建目录并重新执行上面的配置命令，即可采用仓库默认值。需要保留现有构建目录时，先将 `RELEASE_COMMIT` 替换为选定 [arknet release](https://github.com/OpenArkStudio/arknet/releases) 的完整 40 位 commit SHA，再显式更新缓存：

```sh
cmake -S . -B app-build -DARKNET_REVISION=RELEASE_COMMIT
cmake --build app-build --parallel 2
```

已有构建目录会保留其他配置选项。核对 `app-build/CMakeCache.txt` 中的 `ARKNET_REVISION`；使用多配置生成器时，构建仍需添加 `--config Release`。

`ARKNET_SOURCE_DIR` 非空时，CMake 直接使用该本地源码，不会按 `ARKNET_REVISION` 获取版本。开发时需显式更新这个源码目录。需要恢复按固定 commit 获取依赖时，配置 `-DARKNET_SOURCE_DIR=` 并设置所需的 `-DARKNET_REVISION=RELEASE_COMMIT`。这些 CMake 覆盖项仍可使用；CI 策略只核对仓库默认引用，不检查开发者的缓存或本地源码。
