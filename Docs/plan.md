## ARK 计划 2018

- [x] ~~1.spdlog增加按照文件夹/大小/小时滚动的sink(*已经修改了一部分，还需要进一步修改成tlog的格式，binlog/1001/20180627/game1_hour.log*)~~
- [ ] 2.Map, Layer,对应修改协议和Client的处理
- [ ] 3.新架构修改
- [ ] 4.增加连接关系的配置，不再通过程序中手动设置关系(bus_relation, server_settting, IP和port不用再指定了, 通过获取IP和动态生成port), 网络地址修改为tcp://0.0.0.0:9001 wss://0.0.0.0:9001这样的URL
- [ ] 5.增加logserver和logclient插件
- [x ] ~~10.增加网络插件,这样可以将网络模块和业务逻辑模块拆分开(现在NetClient和NetServer还是跟业务关联太紧密了)~~
- [x] ~~6.修改bin文件夹debug和release在一起，debug和release分开配置(方便配置是一套)~~
- [ ] 7.增加运维工具(tcm & deploy)
- [ ] 8.guid改为内部指针类型(降低其他非guid类型的内存占用)
- [ ] 9.修改进程启动优先链接logserver，如果logserver连不上，则直接不能启动，这样就可以从一开始就能打log了

- [ ] 51.优化数据同步的流量,减小属性同步的体积(去掉`NodeName`和`TableName`类似的string同步)
- [ ] 52.拓展DataTable的组成，例如道具包含了宝石(带经验，附魔等)、武器(带随机属性)等东西，DataTable现有的二维结构是特定的，不方便做扩展
- [ ] 53.文档逐步补齐，概念介绍、设计理念、服务器架构、使用例子
- [ ] 54.修改配置打包工具(用Excel编辑，生成**csv**来给工程读取。为解决多人协作冲突, excel可以用名字后缀区分，类如Item.A.xlsx Item.B.xlsx)
- [ ] 55.增加网络库和性能的基准测试用例，放到目录benchmark/unittest
- [ ] 56.脚本模块使用SWIG
- [ ] 57.考虑使用ECS，全部对象Entity化，通过GUID来确定属于哪个分组的哪个位置，能快速定位和查找(通过守望先锋、ET、[ECS-unity](https://github.com/sschmid/Entitas-CSharp)，具体还要再调研)
- [ ] 58.增加coroutine，参考libgo

- [ ] 100.UE4 Demo
  
- [ ] 200.增加监控web页面(vue-admin)
- [ ] 201.增加Deployment web工具
- [ ] 202.增加consul来做服务发现和配置共享(调研后不太适合现在的情况, 考虑etcd/zookeeper来做)  
  

## ARK Bug修改
1. ClearRow的时候后面的行号会变化，所以要考虑要么行号不变，只是置空，要么就得客户端来根据DeleteRow消息同步修改后续的row变化

-------------

Some note:

1. https://www.zhihu.com/question/61169850
2. https://www.zhihu.com/question/23748804/answer/201347615
3. Message.DebugString()