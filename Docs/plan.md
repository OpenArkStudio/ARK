ArkGameFrame计划

- [x] 1.优化数据管理性能
- [x] 2.优化网络性能
- [x] 3.优化heartbeat(Timer)性能，降低精度为秒(现在是毫秒)
- [x] 4.增加Utility插件(放置一些常用功能)
- [ ] 5.增加其他中间件(coroutine, mongoDB)
- [ ] 6.增加通用的proxy-server(参考tconnd, tbus)
- [ ] 7.完整的带业务功能的demo
- [ ] 8.优化数据同步的流量,减小属性同步的体积(去掉`PropertyName`和`RecordName`类似的string同步)
- [ ] 9.拓展record的组成，例如道具包含了宝石(带经验，附魔等)、武器(带随机属性)等东西，record现有的结构是特定的，不方便做扩展
- [ ] 10.增加网络库和性能的基准测试用例，放到目录benchmark/unittest
- [x] 11.CMake添加(`当前进度:` Windows测试完成，linux待测试))
- [ ] 12.增加consul来做服务发现和配置共享(待调研)
- [ ] 13.增加CI和Deployment web工具
- [ ] 14.修改配置打包工具(用Excel编辑，生成csv来给工程读取。为解决多人协作冲突, excel可以用名字后缀区分，类如Item.A.xlsx Item.B.xlsx)
- [ ] 15.[FMT](https://github.com/fmtlib/fmt) FMT_HEADER_ONLY
- [ ] 16.使用SWIG
- [ ] 17.补全Record Callback
- [ ] 18.统一server和插件的目录结构(SDK部分已经统一)
