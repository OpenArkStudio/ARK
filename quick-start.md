# Quick start

## Get the Sources

git clone https://github.com/ArkGame/ARK.git

or

svn checkout https://github.com/ArkGame/ARK

## Dependencies

| Library | Version | License |
| - | - | - |
| [brynet](https://github.com/IronsDu/brynet) | Master | [MIT](https://github.com/IronsDu/brynet/blob/master/LICENSE) |
| [spdlog](https://github.com/gabime/spdlog)  | 0.17.0 | [MIT](https://github.com/gabime/spdlog/blob/v1.x/LICENSE) |
| [google protobuf](https://github.com/google/protobuf) | 3.5.x | [Google License](https://github.com/protocolbuffers/protobuf/blob/master/LICENSE) |
| [args](https://github.com/Taywee/args) | 6.2.0 | [MIT](https://github.com/Taywee/args/blob/master/LICENSE) |
| [rapidxml](http://rapidxml.sourceforge.net) | 1.13 | [LICENSE](http://rapidxml.sourceforge.net/license.txt) |

## Supported Compilers

- GCC 4.9+ (**<font color=red>Recommend</font>. Tested in Ubuntu 15.04**)
- MSVC VS2015up3+ (**<font color=red>Recommend VS2017</font>. Tested in Win7/10**)

## Build and Install

### Windows

##### Prerequisites

- Windows 7 or later
- Visual Studio 2017
- Git
- CMake

##### Step

Using Visual Studio 2017

1. Pull all source
2. Run command line 

```batch
cd path/to/ARK
cd Dep
build_dep.bat
```

3. Open the solution: `Frame/ARK.sln`
4. Build the solution`ARK.sln`
5. Run `ARK\Bin\resource\tool\gen-config.bat` to generate configuration files
6. Run the binary file by `Bin/rund.bat`

Using CMake

1. Pull all source
2. Run command line

```batch
cd path/to/ARK
cd Dep
build_dep.bat
cd ../
md build
cd build
cmake -G "Visual Studio 15 Win64" ..
```

4. Start `ARK.sln`
5. Run `ARK\Bin\resource\tool\gen-config.bat` to generate configuration files
6. Run the binary file by `Bin/rund.bat`

!> If you are using VS2017, please change `%VS150COMNTOOLS%` in `Dep/build_dep.bat` to `YOUR-COMPILER-PATH`.</br>
If you DO NOT install **Win10 SDK**, please use **CMake** to create new solution.

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
cd Dep
./build_dep.sh
cd ../
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=debug ..
make
```

!> If you need build release version, please add `-DCMAKE_BUILD_TYPE=release` in cmake command.</br>
To accelerate the building speed, you can use `make -j` to run multiple complication jobs simultaneously. However, that may exhaust the memory.

3. Run `ARK\Bin\resource\tool\gen-config.sh` to generate configuration files

!> *Cuz our Tools is written by C#, so please generate your configuration in Windows, then transfer to Linux. we plan to use .Net Core to create a new version in Linux.*

4. Run the binary file by `Bin/rund.sh`