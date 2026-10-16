# 安装与构建

ark 需要 CMake 3.21+、Ninja、支持 C++20 的编译器，以及 vcpkg manifest 选定的依赖。Windows 使用 Visual Studio 2022 Developer PowerShell，macOS/Linux 使用兼容的 Clang 或 GCC 工具链。

<Steps>
<Step title="克隆仓库并初始化依赖">

```sh
git clone https://github.com/OpenArkStudio/ark.git
cd ark
git submodule update --init --recursive
```

</Step>
<Step title="配置应用构建">

安装 vcpkg，将环境变量 `VCPKG_ROOT` 指向对应目录。网络依赖选择 Boost。

macOS/Linux 使用 POSIX shell：

```sh
cmake -S . -B app-build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_MANIFEST_FEATURES="application;boost" \
  -DASIO_STANDALONE=OFF -DCMAKE_BUILD_TYPE=Release
```

Windows 使用 Visual Studio 2022 Developer PowerShell：

```powershell
cmake -S . -B app-build -G Ninja `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  "-DVCPKG_MANIFEST_FEATURES=application;boost" `
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
