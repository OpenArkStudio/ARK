![Server Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Docs/asserts/imgs/ArkGameFrame.png)

[![GitHub version](https://badge.fury.io/gh/ArkGame%2FArkGameFrame.svg)](https://badge.fury.io/gh/ArkGame%2FArkGameFrame)
[![Build Status](https://travis-ci.org/ArkGame/ArkGameFrame.svg?branch=master)](https://travis-ci.org/ArkGame/ArkGameFrame)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Platform](https://img.shields.io/badge/platform-Linux,%20Windows-green.svg?style=flat)](https://github.com/ArkGame/ArkGameFrame)
[![Gitter chat](https://img.shields.io/gitter/room/nwjs/nw.js.svg)](https://gitter.im/ArkGame/Lobby)    
**Copyright © [ARK Game](https://arkgame.net "ARK Game")**    
Website: https://arkgame.net  
Github: https://github.com/ArkGame/ArkGameFrame  
Gitee(码云): https://gitee.com/ArkGame/ArkGameFrame  

##### Wanna get the newest features? Please switch to the develop branch.

**<font color=red>If you wanna commit PR or report issue, please refer to [Contribution Guide](https://github.com/ArkGame/ArkGameFrame/blob/master/CONTRIBUTING.md)</font>**.

## Contact

- QQ Group: 540961187
- mail: arkgametech@163.com

## ArkGameFrame
ArkGameFrame is a lightweight, agility, scalable, distributed plugin framework written by C++，make it easier and faster to create your own game server, can be used to develop MMO RPG, ARPG, Round play, chess games. Inspired by UnrealEngine and Ogre.

## Features

- Flexible apps, plugins, and modules
- High availability architectures
- General abstract data system
- Interface-oriented and data-oriented programming
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

Please [create a new issue](https://github.com/ArkGame/ArkGameFrame/issues) to ask a question.


## Architecture

### App Architecture:

![App Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Docs/asserts/imgs/AppArchitecture.png)

## Get the Sources:

git clone https://github.com/ArkGame/ArkGameFrame.git

or

svn checkout https://github.com/ArkGame/ArkGameFrame

## Dependencies

- libevent master
- easylogging++ 9.80
- mysql++ 3.2.2
- google protobuf 2.7.0
- evpp 0.7.0

## Supported Compilers

- GCC >= 4.8 (**<font color=red>Recommend</font>. Tested in Ubuntu 15.04**)
- MSVC >= VS2015 update 3 (**<font color=red>Recommend</font>. Tested in Win7/10**)

## Build and Install

### Windows

##### Prerequisites:

- Windows 10, 8.1, or 7
- Visual Studio 2017 or Visual Studio 2015 Update 3
- Git
- CMake 3.0 +

##### Step:
Using Visual Studio 2015
1. Pull all source
2. Run command line 
```batch
cd path/to/ArkGameFrame
cd Dep
build_dep.bat
```
3. Open the solution: `Frame/ARK.sln`
4. Build the solution`ARK.sln`
5. Run `GenerateConfigXML.bat` to generate configuration files
6. Run the binary file by `Bin/Server/Debug/Run.bat`

Using CMake
1. Pull all source
2. Run command line
```batch
cd path/to/ArkGameFrame
cd Dep
build_dep.bat
cd ../
md build
cd build
cmake -G "Visual Studio 14 Win64" ..
```
4. Start `ARK.sln`

> Note. If you are using VS2017, please change **%VS140COMNTOOLS%** in **Dep/build_dep.bat** to **YOUR-COMPILER-PATH**.

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
make -j
```
> Note. If you need build debug, please add `-DCMAKE_BUILD_TYPE="Debug"` in cmake command.
3. Run `bash ./GenerateConfigXML.sh` to generate configuration files
4. Run the binary file by `Bin/Server/Debug/rund.sh`

## Documents

[English](https://github.com/ArkGame/ArkGameFrame/blob/master/Docs/doc_EN.md)

[Mandarin](https://github.com/ArkGame/ArkGameFrame/blob/master/Docs/doc_ZH.md)

## Examples

Please see `Frame/Examples`.

## Plan

View our [lastest plan](https://github.com/ArkGame/ArkGameFrame/blob/master/Docs/plan.md)

## Unity3D client

[Unity3D Client](https://github.com/ArkGame/ArkClient-Unity3D)

## License

The ArkGameFrame project is currently available under the [Apache License](https://github.com/ArkGame/ArkGameFrame/blob/master/LICENSE).

----------

Our team has always specialized in the agile development and try to help different teams to develop game project quickly and easily. After years of development, we create this frame. We are still improving it.

----------

## Some Awesome projects

[PSS](https://github.com/freeeyes/PSS)
- Author: freeeyes
- Description: 基于插件的跨平台网络服务器框架

----------

#### The relationship with NF

This branch come from [NF](https://github.com/ketoo/NoahGameFrame), we used to be the **CORE DEVELOPERS** of NF. We create **ArkGameFrame** because some different design philosophies, through our new ideas and modifications, now this project is a fresh project which has many different features.
