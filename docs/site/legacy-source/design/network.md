# 新的改进设计思路

## 架构设计

分为三层

- cluster 全区全服进程
- router  zone <-> cluster的中间层，主要做协议转发
- zone    小区进程

## 网络拓扑设计

zone 内的网络拓扑均由world来管理
cluster 内的网络拓扑均由master来管理

zone内的消息内部消化

zone发往cluster的消息，可以直接发往router，由router根据包头中的dst_bus_id来转发

cluster的所有进程都需要连master和router

zone内的部分进程链接router

## log设计

所有的进程都需要先链接log server后才能开始业务功能，这样可以保证所有的log都输出到远端去，log client插件需要有一个配置，表示远端log server ip-port，以及log格式