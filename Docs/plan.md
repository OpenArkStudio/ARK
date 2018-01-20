ArkGameFrame计划 2018

- [ ] 1.统一名称为**Entity**, **DataNode**, **DataTable**, Map, Layer,对应修改协议和Client的处理
- [ ] 2.更新pb为3.0+
- [ ] 3.优化数据同步的流量,减小属性同步的体积(去掉`PropertyName`和`RecordName`类似的string同步)
- [ ] 4.拓展record的组成，例如道具包含了宝石(带经验，附魔等)、武器(带随机属性)等东西，record现有的结构是特定的，不方便做扩展
- [ ] 5.文档逐步补齐，概念介绍、设计理念、服务器架构、使用例子
- [ ] 6.修改配置打包工具(用Excel编辑，生成csv来给工程读取。为解决多人协作冲突, excel可以用名字后缀区分，类如Item.A.xlsx Item.B.xlsx)
- [ ] 7.增加网络库和性能的基准测试用例，放到目录benchmark/unittest
- [ ] 8.[FMT](https://github.com/fmtlib/fmt) FMT_HEADER_ONLY
- [ ] 9.修改log库使用[spdlog](https://github.com/gabime/spdlog)
- [ ] 10.脚本模块使用SWIG
- [ ] 11.考虑使用ECS，全部对象Entity化，通过GUID来确定属于哪个分组的哪个位置，能快速定位和查找(通过守望先锋和[ECS-unity](https://github.com/sschmid/Entitas-CSharp)并没有看出来是如何处理这种大量查找的，这个方法是和freeeyes讨论的结果)
  
- [ ] 100.完整的带业务功能的demo  
  
- [ ] 200.增加监控web页面(vue-admin)
- [ ] 201.增加Deployment web工具
- [ ] 202.增加consul来做服务发现和配置共享(待调研)  
  

ArkGameFrame Bug修改

- [ ] 1.NetModule没有释放问题
- [ ] 2.服务器先断开，NetClient会有assert的问题
- [ ] 3.Record ReleaseAll的时候delete row_data会dump