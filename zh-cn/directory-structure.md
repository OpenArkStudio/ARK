# 目录结构说明

?> ARK 源码目录结构

```
|-- Bin                         #二进制目录
|   |-- bin                     #可执行文件目录, Launcher
|   |-- binlog                  #log输出目录
|   |-- bus_conf                #bus配置目录
|   |-- lib                     #第三方动态库和内部插件生成的目录
|   |-- plugin_conf             #进程使用哪些插件的配置路径
|   |-- resource                #设计人员配置目录
|   |   |-- client              #给客户端用的csv文件目录, 工具生成
|   |   |-- excel               #设计人员配置的excel文件目录
|   |   |-- mysql               #MySQL表结构路径, 工具生成
|   |   |-- proto               #DataNode和DataTable名字文件目录, 工具生成
|   |   |-- res                 #工具将excel转为xml的目录
|   |   |-- schema              #工具将excel样式结构转为xml结构的目录
|   |   |-- script              #lua脚本文件(暂时未使用)
|   |-- tools                   #excel文件转换工具
|-- cmake                       #第三方cmake依赖文件
|-- Dep                         #第三方开源库
|   |-- args                    #命令行参数解析库
|   |-- common                  #一些开源的head only的文件
|   |-- rapidxml                #操作xml的库
|   |-- spdlog                  #生成log的库
|-- Frame                       #框架源代码
|   |-- Common                  #一些抽出来的公共头文件
|   |-- Examples                #框架使用例子
|   |-- Launcher                #加载插件的启动器
|   |-- SDK                     #公共的插件和静态库, 基本会被每个插件用到
|   |   |-- BusPlugin           #网络总线插件, 用来提供网络链接、消息发送等
|   |   |-- Core                #Core静态库, 主要包含内部用到的数据结构
|   |   |-- Interface           #SDK中插件的接口文件
|   |   |-- KernelPlugin        #Kernel插件, 主要管理Entity的数据, 非常关键
|   |   |-- NetPlugin           #网络插件, 提供TCP、http、websocket服务端和客户端服务
|   |   |-- Proto               #内部用到的protobuf IDL文件
|   |   |   |-- cpp
|   |   |   |-- cs
|   |   |   |-- js
|   |   |   |-- proto
|   |   |   |-- proto-gen
|   |   |-- UtilityPlugin       #Utility插件, 提供一些常用模块, 例如GUID, 本地log等
|   |-- Server                  #服务插件
|   |   |-- DBPlugin            #数据库代理插件
|   |   |-- DirPlugin           #客户端目录入口插件
|   |   |-- GamePlugin          #游戏插件, 包含了所有的业务玩法内容
|   |   |-- Interface           #server中插件的接口文件
|   |   |-- LoginPlugin         #登录插件
|   |   |-- LogPlugin           #远端log插件
|   |   |-- MasterPlugin        #中心插件, 主要用来管理其他进程
|   |   |-- OSSPlugin           #运营插件, 为运营人员提供游戏外操作游戏内数据的功能
|   |   |-- ProxyPlugin         #网络桥接插件, 提供客户端和服务端双向通讯流量转发
|   |   |-- RouterPlugin        #路由插件, 用来转发cluster和zone层的通讯流量
|   |   |-- WorldPlugin         #world插件, 用来管理zone内部的进程
|   |-- Tools                   #自用工具
|       |-- Conv2UTF8           #将文件转化为UTF8编码
|       |-- FileProcessor       #将excel文件转化为xml文件
|-- Plugins                     #我们提供的或其他开源者提供的插件, 例如mysql, redis, 等插件.
`-- Test                        #一些测试用例的代码(暂时还没有使用)
```