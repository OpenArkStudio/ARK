# 快速开始

## 获取源码

git clone https://github.com/ArkGame/ARK.git

or

svn checkout https://github.com/ArkGame/ARK

## 依赖库
| 库名 | 版本 | 许可协议 |
| - | - | - |
| [brynet](https://github.com/IronsDu/brynet) | Master | [MIT](https://github.com/IronsDu/brynet/blob/master/LICENSE) |
| [spdlog](https://github.com/gabime/spdlog)  | 0.17.0 | [MIT](https://github.com/gabime/spdlog/blob/v1.x/LICENSE) |
| [google protobuf](https://github.com/google/protobuf) | 3.5.x | [Google License](https://github.com/protocolbuffers/protobuf/blob/master/LICENSE) |
| [args](https://github.com/Taywee/args) | 6.2.0 | [MIT](https://github.com/Taywee/args/blob/master/LICENSE) |
| [rapidxml](http://rapidxml.sourceforge.net) | 1.13 | [LICENSE](http://rapidxml.sourceforge.net/license.txt) |

## 支持的编译器

- GCC 4.9+ (**<font color=red>Recommend</font>. Tested in Ubuntu 15.04**)
- MSVC VS2015up3+ (**<font color=red>Recommend VS2017</font>. Tested in Win7/10**)

## 编译和安装

### Windows

##### 前提条件

- Windows 7 or later
- Visual Studio 2017
- Git
- CMake 3.0+

##### 步骤

使用Visual Studio 2017

1. 克隆源码
2. 按步骤运行下面命令

```batch
cd path/to/ARK
cd Dep
build_dep.bat
```

3. 打开解决方案文件 `Frame/ARK.sln`
4. 编译 `ARK.sln`
5. 运行 `ARK\Bin\resource\tool\gen-config.bat`生成配置文件
6. 运行脚本 `Bin/rund.bat`

------

使用cmake

1. 克隆源码
2. 按步骤运行下面命令

```batch
cd path/to/ARK
cd Dep
build_dep.bat
cd ../
md build
cd build
cmake -G "Visual Studio 15 Win64" ..
```

4. 编译 `build\ARK.sln`
5. 运行 `ARK\Bin\resource\tool\gen-config.bat` 生成配置文件
6. 运行脚本 `Bin/rund.bat`

> [!NOTE]
> 请提前在`Dep/build_dep.bat`中设置`%VS150COMNTOOLS%`变量为`YOUR-COMPILER-PATH` 或 增加系统环境变量.</br>
如果你没有安装 **Win10 SDK**, 请使用 **CMake** 创建解决方案文件.

### linux

##### 前提条件

- g++ 4.9+ (需要 C++ 11 和 std::regex)
- Git
- CMake 3.0+
- libtool
- automake

使用cmake

1. 克隆源码
2. 按步骤在终端运行下面命令

```shell
cd Dep
./build_dep.sh
cd ../
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=debug ..
make
```

> [!NOTE]
> 如果你想编译`release`版本, 请使用`-DCMAKE_BUILD_TYPE=release`参数.</br>
为了加速编译, 你可以使用 `make -j` 开启多核编译. 不过可能会消耗完你的系统内存.

3. 运行 `ARK\Bin\resource\tool\gen-config.sh` 生成配置文件

> [!WARNING]
> *因为我们的工具是使用C#开发的, 所以暂时请在Windows环境下生成后再放入Linux系统对应目录. 我们计划使用`.Net Core`开发一个跨平台的工具.*

4. 运行脚本 `Bin/rund.sh`