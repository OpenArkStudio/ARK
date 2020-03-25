# 快速开始

## 获取源码

git clone https://github.com/ArkNX/ARK.git

or

svn checkout https://github.com/ArkNX/ARK

## 依赖库
| 库名                                                  | 版本   | 许可协议                                                                          |
| ----------------------------------------------------- | ------ | --------------------------------------------------------------------------------- |
| [brynet](https://github.com/IronsDu/brynet)           | Master | [MIT](https://github.com/IronsDu/brynet/blob/master/LICENSE)                      |
| [spdlog](https://github.com/gabime/spdlog)            | 0.17.0 | [MIT](https://github.com/gabime/spdlog/blob/v1.x/LICENSE)                         |
| [google protobuf](https://github.com/google/protobuf) | 3.5.x  | [Google License](https://github.com/protocolbuffers/protobuf/blob/master/LICENSE) |
| [args](https://github.com/Taywee/args)                | 6.2.0  | [MIT](https://github.com/Taywee/args/blob/master/LICENSE)                         |
| [rapidxml](http://rapidxml.sourceforge.net)           | 1.13   | [LICENSE](http://rapidxml.sourceforge.net/license.txt)                            |

## 支持的编译器

- GCC 4.9+ (**<font color=red>Recommend</font>. Tested in Ubuntu 15.04**)
- MSVC VS2017 (**<font color=red>Recommend VS2019</font>. Tested in Win7/10**)

## 编译和运行

### Windows

##### 前置准备

- Windows 7 以上
- Visual Studio 2017
- Git
- CMake 3.0+

##### 步骤

使用cmake

1. 克隆源码
2. 按步骤运行下面命令

    ```batch
    cd path/to/ARK
    git submodule update --init
    cd dep
    md build
    cd build
    cmake -G "Visual Studio 16" -A x64 ..
    ```
    
3. 编译 `ark.sln`
4. 运行 `path/to/ARK/bin/tools/gen-config.bat` 生成配置文件
> [!WARNING]
> 需要安装Python3环境，需要的第三方库请查看`ARK/bin/tools/config_tool/README.md`

5. 安装[Consul](https://consul.io), 启动命令为:
  
   ```bash
   cd path/to/consul
   consul.exe agent -server -ui -bootstrap-expect=1 -data-dir=./ -node=ark_consul_service -client=0.0.0.0 -bind=127.0.0.1 -log-file=./log/ -datacenter=ARK
   ```

6. 运行脚本 `bin/rund.bat`

> [!NOTE]
> 如果你使用其他Visual Studio版本, 请将 `dep/build_dep.bat`中的`%VS160COMNTOOLS%`变量 和 `CMake命令行` 替换为正确的Visual Studio版本和路径.</br>

### Linux

##### 前置准备

- g++ 4.9+ (需要 C++ 11 和 std::regex)
- Git
- CMake 3.0+
- libtool
- automake

使用cmake

1. 克隆源码
2. 按步骤在终端运行下面命令

    ```shell
    cd path/to/ARK
    git submodule update --init
    mkdir -p build/debug && cd build/debug
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=debug ../../
    make
    ```

> [!NOTE]
> 如果你想编译`release`版本, 请使用`-DCMAKE_BUILD_TYPE=release`参数.</br>
为了加速编译, 你可以使用 `make -j [num]` 开启多核编译. 不过可能会消耗完你的系统内存.

3. 运行 `path/to/ARK/bin/tools/gen-config.sh` 生成配置文件

> [!WARNING]
> 需要安装Python3环境，需要的第三方库请查看`ARK/bin/tools/config_tool/README.md`

4. 安装[Consul](https://consul.io), 启动命令为:
  
   ```shell
   cd path/to/consul
   ./consul agent -server -ui -bootstrap-expect=1 -data-dir=./ -node=ark_consul_service -client=0.0.0.0 -bind=127.0.0.1 -log-file=./log/ -datacenter=ARK
   ```

5. 运行脚本 `bin/rund.sh`