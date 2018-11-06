# Concepts

## Module(模块)
表示一类逻辑业务的合集, 相对来说功能比较集中, 可以做到低耦合, 并且可以通过`IOP`(面向接口编程)的方式来给其他模块提供耦合功能.例如LogModule等.

## Plugin(插件)
表示一系列Module的集合, 按照更大的业务来分类, 例如GameLogic插件, Navimesh插件等.

## Application(应用)
表示一个独立的完整功能的进程, 可以包含大量插件, 例如GameServer等.

## DataNode
表示一维数据, 通常用来表示Entity附带的任意一维数据结构, 当前可以为常用内置数据类型(`bool` `int` `int64` `double` `float` `string` `GUID`). 例如`PlayerEnity`附带的HP MP等数据.

## DataTable
表示二维数据, 通常用来表示Entity附带的任意二维数据结构,结构与Microsoft Excel的二维结构类似, 包含`Row`和`Column`, 并且结构可以通过Excel动态传入, Table中任意Cell可以为常用内置数据类型(`bool` `int` `int64` `double` `float` `string` `GUID`). 例如`PlayerEnity`附带的`ItemTable`.

## Entity(实体)
表示游戏内动态创建的任意实体对象, 该实体对象通常携带大量`DataNode`和`DataTable`, 例如：PlayerEntity、MonsterEntity、NPCEntity等.

## Launcher
用来通过加载多个Plugin动态链接库, 从而形成一个Application.

## GUID
表示游戏内唯一ID, 并且全区全服不重复, 可以用来做`EntityID`等游戏内有唯一性需求的ID.

## Event(事件)
游戏逻辑监听和产生事件, 用来解耦游戏逻辑.

## Google Proto-buffer
Google开源的跨平台, 跨语言的数据序列化库, 详见https://github.com/google/protobuf