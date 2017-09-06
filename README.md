![Server Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Doc/asserts/imgs/ArkGameFrame.png)

## ArkGameFrame
ArkGameFrame is a lightweight, fast, scalable, distributed plugin framework written in C++，make it easier to create game server. Greatly inspired by OGRE and Bigworld. It is a fork of the NFrame project.
##### Wanna get the newest features? Please switch to the develop branch.

## contact
- QQ Group:540961187
- mail:arkgametech@163.com

## Features
- General abstract data system
- Interface-oriented programming (IOP)
- Event-driven and data-driven
- Extensible apps, plugins, and modules
- High concurrency and performance by using actor model (Theron)
- Based on the event-driven and attribute-driven can make business more clear and easy to maintenance
- Based on C++ standard, easy to handle and learn
- Cross platform (Include Windows and Linux)
- Distributed server architecture
- Use Excel as configuration files, easier for users
- With existed Unity3D client for rapid development

### FAQ
Please create a new issue to ask a question.

### Unity3D client
[Unity3D Client](https://github.com/ArkGame/ArkClient-Unity3D)

## Architecture

### App Architecture:
![App Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Doc/asserts/imgs/AppArchitecture.png)

### Server Architecture
![Server Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Doc/asserts/imgs/ServerArchitecture.png)

## Get the Sources:

git clone https://github.com/ArkGame/ArkGameFrame.git

or

svn checkout https://github.com/ArkGame/ArkGameFrame


## Dependencies

- libevent 2.0.22
- easylogging++ 9.80
- mysql++ 3.2.2
- google protobuf 2.5.0
- pybind11 1.2
- redis-cpp-client 
- Theron 6.00.01
- curl 7.37.1
- evpp 0.7.0

## Supported Compilers

* GCC >= 4.8 (**<font color=red>Recommend</font> Tested in Ubuntu 15.04**)
* MSVC >= VS2015 (**<font color=red>Recommend</font> Tested in Win7/10**)
* MSVC >= VS2012+ & GCC >= 4.4+ (By using ```boost```, define macro ```HAVE_BOOST```, then you need to add boost include files and libraries yourself, just to replace ```boost::variant``` and ```boost::lexical_cast```)

## Build and Install
### MSVC >= 2015

1. Git pull all source
2. Run ```Dep/build_dep.bat```
3. Open the solution: ```Frame/Ark.sln```
4. Build the solution```Ark.sln```
5. Run ```GenerateConfigXML.bat``` to generate configuration files
6. Run the binary file by ```Bin/Server/Debug/Run.bat```

### CodeBlocks >= 13.1(Only in linux)

1. Git pull all source
2. Run ```Dep/build_dep.sh```
3. Open the solution with CodeBlocks: ```Frame/Ark.workspace```
4. Check ```C++11``` option in CodeBlocks compiler setting
5. Build ```Ark.workspace```
6. Run ```bash ./GenerateConfigXML.sh``` to generate configuration files
6. Run the binary file by ```Bin/Server/Debug/rund.sh```

## Documents
[User Manual](https://arkgame.github.io/ArkGameFrame/)

[English](https://github.com/ArkGame/ArkGameFrame/blob/master/Doc/doc_EN.md)

[Mandarin](https://github.com/ArkGame/ArkGameFrame/blob/master/Doc/doc_ZH.md)

## Examples
Please see ```Frame/Examples```.

## License
The ArkGameFrame project is currently available under the [Apache License](https://github.com/ArkGame/ArkGameFrame/blob/master/LICENSE).


----------
### C# Project
Still working on it, maybe use Akka.net or Orleans

----------
Our team has always specialized in the agile development and try to help different teams to develop game project quickly and easily. After years of development, we create this frame which is a fast, scalable, distributed game server framework written by C++, include the actor library, network library, can be used as a real-time multiplayer game engine. We are still improving it.

----------

#### Plan
- [ ] 1.优化数据管理性能
- [x] 2.优化网络性能
- [ ] 3.优化heartbeat(Timer)性能，降低精度为秒(现在是毫秒)
- [x] 4.增加Utility插件(放置一些常用功能)
- [ ] 5.增加其他中间件(coroutine, mongoDB)
- [ ] 6.增加通用的proxy-server(参考tconnd, tbus)
- [ ] 7.完整的带业务功能的demo
- [ ] 8.优化数据同步的流量,减小属性同步的体积(去掉```PropertyName```和```RecordName```类似的string同步)
- [ ] 9.拓展record的组成，例如道具包含了宝石(带经验，附魔等)、武器(带随机属性)等东西，record现有的结构是特定的，不方便做扩展
