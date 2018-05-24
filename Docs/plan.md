## ARK 计划 2018

- [ ] 1.spdlog增加按照文件夹/大小/小时滚动的sink
- [ ] 2.Map, Layer,对应修改协议和Client的处理
- [ ] 3.优化数据同步的流量,减小属性同步的体积(去掉`NodeName`和`TableName`类似的string同步)
- [ ] 4.拓展DataTable的组成，例如道具包含了宝石(带经验，附魔等)、武器(带随机属性)等东西，DataTable现有的二维结构是特定的，不方便做扩展
- [ ] 5.文档逐步补齐，概念介绍、设计理念、服务器架构、使用例子
- [ ] 6.修改配置打包工具(用Excel编辑，生成**csv**来给工程读取。为解决多人协作冲突, excel可以用名字后缀区分，类如Item.A.xlsx Item.B.xlsx)
- [ ] 7.增加网络库和性能的基准测试用例，放到目录benchmark/unittest
- [ ] 10.脚本模块使用SWIG
- [ ] 11.考虑使用ECS，全部对象Entity化，通过GUID来确定属于哪个分组的哪个位置，能快速定位和查找(通过守望先锋、ET、[ECS-unity](https://github.com/sschmid/Entitas-CSharp)，具体还要再调研)
- [ ] 12.增加coroutine，参考libgo
- [ ] 13.增加连接关系的配置，不再通过程序中手动设置关系

- [ ] 100.完整的带业务功能的demo  
  
- [ ] 200.增加监控web页面(vue-admin)
- [ ] 201.增加Deployment web工具
- [ ] 202.增加consul来做服务发现和配置共享(待调研)  
  

## ARK Bug修改

-------------

Some note,

1. https://www.zhihu.com/question/61169850
2. https://www.zhihu.com/question/23748804/answer/201347615