![Server Architecture](https://raw.githubusercontent.com/ArkGame/ARK/master/Docs/asserts/imgs/ArkGameFrame.png)

[![GitHub version](https://badge.fury.io/gh/ArkGame%2FARK.svg)](https://badge.fury.io/gh/ArkGame%2FARK)
![GitHub stars](https://img.shields.io/github/stars/ArkGame/ARK.svg?style=flat-square&label=Stars)
![GitHub issues](https://img.shields.io/github/issues-raw/ArkGame/ARK.svg?style=flat-square)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/ArkGame/ARK.svg?style=flat-square)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++11](https://img.shields.io/badge/C++-11-4c7e9f.svg)](https://github.com/ArkGame/ARK)
[![Platform](https://img.shields.io/badge/platform-Linux,%20Windows-green.svg?style=flat)](https://github.com/ArkGame/ARK)
[![SonarCloud](https://sonarcloud.io/api/project_badges/measure?project=ark&metric=alert_status)](https://sonarcloud.io/dashboard/index/ark)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1916/badge)](https://bestpractices.coreinfrastructure.org/projects/1916)
[![Gitter](https://img.shields.io/gitter/room/nwjs/nw.js.svg)](https://gitter.im/ArkGame/Lobby)

| Branch | Build Status |
| - | - |
| master | [![Build Status](https://travis-ci.org/ArkGame/ARK.svg?branch=master)](https://travis-ci.org/ArkGame/ARK) |
| develop | [![Build Status](https://travis-ci.org/ArkGame/ARK.svg?branch=develop)](https://travis-ci.org/ArkGame/ARK) |

**Copyright © [ARK Game](https://arkgame.net "ARK Game")**   
Website: https://arkgame.net  
Github: https://github.com/ArkGame/ARK  
Gitee(码云): https://gitee.com/ArkGame/ARK  
QQ群: [540961187](https://shang.qq.com/wpa/qunwpa?idkey=1b8394bd9a42ba46606200a44911c1c6161235a38aecce95158ca646c2bafd81)

##### Wanna get the newest features? Please switch to the develop branch.

**<font color=red>If you wanna commit PR or report issue, please refer to [Contribution Guide](https://github.com/ArkGame/ARK/blob/master/PULL_REQUEST_TEMPLATE.md)</font>**.

## ARK
ARK is a lightweight, agility, scalable, distributed plugin framework written by C++，make it easier and faster to create your own game server, can be used to develop MMO RPG, ARPG, Round play, chess games. Inspired by UnrealEngine and Ogre.

## Features

- Flexible apps, plugins, and modules
- High availability architectures
- General abstract data system
- Interface-oriented and data-oriented programming(IOP & DOP)
- Event-driven and data-driven
- High concurrency and performance of network
- Based on C++ standard, easy to handle and learn
- Cross platform (Include Windows and Linux)
- Less downtime and higher availability
- Lower training and education costs
- Use Excel as configuration files, easier for game designer
- With existed simple Unity3D client for rapid development
- Plentiful plugins(DB, script, http, websocket, etc.)
- Customization service for business customer

### FAQ

Please [create a new issue](https://github.com/ArkGame/ARK/issues) OR [visit our community](https://arkgame.net/questions) to ask a question.


## Architecture

### App Architecture:

![App Architecture](https://raw.githubusercontent.com/ArkGame/ARK/master/Docs/asserts/imgs/AppArchitecture.png)

## Get the Sources:

git clone https://github.com/ArkGame/ARK.git

or

svn checkout https://github.com/ArkGame/ARK

## Dependencies

- [brynet](https://github.com/IronsDu/brynet) MIT
- [spdlog](https://github.com/gabime/spdlog) MIT
- [mysql++](https://tangentsoft.com/mysqlpp/home)
- [google protobuf 3.5.x](https://github.com/google/protobuf)

## Supported Compilers

- GCC >= 4.8 (**<font color=red>Recommend</font>. Tested in Ubuntu 15.04**)
- MSVC >= VS2015 update 3 (**<font color=red>Recommend VS2017</font>. Tested in Win7/10**)

## Build and Install

### Windows

##### Prerequisites:

- Windows 7 or later
- Visual Studio 2017
- Git
- CMake

##### Step:
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
5. Run `ARK\Bin\Server\DataConfig\Tool\gen-config.bat` to generate configuration files
6. Run the binary file by `Bin/Server/Debug/Run.bat`

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
5. Run `ARK\Bin\Server\DataConfig\Tool\gen-config.bat` to generate configuration files
6. Run the binary file by `Bin/Server/Debug/Run.bat`

> Note
> - If you are using VS2015, please use `Dep/build_dep_vs2015.bat`
> - If you are using VS2017, please change `%VS150COMNTOOLS%` in `Dep/build_dep.bat` to `YOUR-COMPILER-PATH`.
> - If you DO NOT install **Win10 SDK**, please use **CMake** to create new solution.

### linux

##### Prerequisites:

- g++ 4.8+ (need C++ 11)
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
cmake -G "Unix Makefiles" ..
make
```
> Note
> - If you need build debug, please add `-DCMAKE_BUILD_TYPE="Debug"` in cmake command.
> - To accelerate building, you can use `make -j` to run multiple complication jobs simultaneously. However, that may exhaust the memory.
3. Run `ARK\Bin\Server\DataConfig\Tool\gen-config.sh` to generate configuration files(*Cuz our Tools is written by C#, so please generate your configuration in Windows, we plan to use .Net Core to create a new version in Linux*)
4. Run the binary file by `Bin/Server/Debug/rund.sh`

## Documents

[English](https://github.com/ArkGame/ARK/blob/master/Docs/doc_EN.md)

[Mandarin](https://github.com/ArkGame/ARK/blob/master/Docs/doc_ZH.md)

## Examples

Please see `Frame/Examples`.

## Plan

View our [lastest plan](https://github.com/ArkGame/ARK/blob/master/Docs/plan.md)

## Unity3D client

[Unity3D Client](https://github.com/ArkGame/ArkClient-Unity3D)

## License

The ARK project is currently available under the [Apache License](https://github.com/ArkGame/ARK/blob/master/LICENSE).

----------

Our team has always specialized in the agile development and try to help different teams to develop game project quickly and easily. After years of development, we create this frame. We are still improving it.

----------

## Some Awesome projects

[PSS](https://github.com/freeeyes/PSS)
- Author: freeeyes
- Description: 基于插件的跨平台网络服务器框架

----------

#### The relationship with NF

This branch come from [NF](https://github.com/ketoo/NoahGameFrame), we used to be the **CORE DEVELOPERS** of NF. We create **ARK** because some different design philosophies, through our new ideas and modifications, now this project is a fresh project which has many different features.
