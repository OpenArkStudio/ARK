# Quick start

## Get the Sources

git clone https://github.com/ArkGame/ARK.git

or

svn checkout https://github.com/ArkGame/ARK

## Dependencies

| Library                                               | Version | License                                                                           |
| ----------------------------------------------------- | ------- | --------------------------------------------------------------------------------- |
| [brynet](https://github.com/IronsDu/brynet)           | master  | [MIT](https://github.com/IronsDu/brynet/blob/master/LICENSE)                      |
| [spdlog](https://github.com/gabime/spdlog)            | 0.17.0  | [MIT](https://github.com/gabime/spdlog/blob/v1.x/LICENSE)                         |
| [google protobuf](https://github.com/google/protobuf) | 3.5.x   | [Google License](https://github.com/protocolbuffers/protobuf/blob/master/LICENSE) |
| [args](https://github.com/Taywee/args)                | 6.2.0   | [MIT](https://github.com/Taywee/args/blob/master/LICENSE)                         |
| [rapidxml](http://rapidxml.sourceforge.net)           | 1.13    | [LICENSE](http://rapidxml.sourceforge.net/license.txt)                            |
| [filesystem](https://github.com/wjakob/filesystem)    | master  | [self license](https://github.com/wjakob/filesystem/blob/master/LICENSE)          |

## Supported Compilers

- GCC 4.9+ (**<font color=red>Recommend</font>. Tested in Ubuntu 15.04**)
- MSVC VS2015up3+ (**<font color=red>Recommend VS2017</font>. Tested in Win7/10**)

## Build and Launch

### Windows

##### Prerequisites

- Windows 7 or later
- Visual Studio 2017
- Git
- CMake

##### Step

Using CMake

1. Pull all source
2. Run command line

```batch
cd path/to/ARK
cd dep
build_dep.bat
cd ../
md build
cd build
cmake -G "Visual Studio 15 Win64" ..
```

3. Start `ark.sln`
4. Run `ARK/bin/tools/gen-config.bat` to generate configuration files
5. Run the binary file by `bin/rund.bat`

> [!NOTE]
> If you are using other Visual Studio version, please replace `%VS150COMNTOOLS%` in `dep/build_dep.bat` and `cmake -G "Visual Studio 15 Win64" ..` with the right Visual Studio version and path. </br>

### linux

##### Prerequisites

- g++ 4.9+ (need C++ 11 and std::regex)
- Git
- CMake 3.0+
- libtool
- automake

Using CMake

1. pull all source
2. Use bash

```shell
cd dep
./build_dep.sh
cd ../
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=debug ..
make
```

> [!NOTE]
> If you need build release version, please add `-DCMAKE_BUILD_TYPE=release` in cmake command.</br>
To accelerate the building speed, you can use `make -j [num]` to run multiple complication jobs simultaneously. However, that may exhaust the memory.

3. Run `ARK/bin/tools/gen-config.sh` to generate configuration files

> [!WARNING] 
> Need install python first

4. Run the binary file by `bin/rund.sh`