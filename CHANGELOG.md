# ARK changelogs

### version 0.9

- Fix `HeartBeatManager` remove mistake(Thanks @sks)
- Add `DateTime`
- Fix sonar bad smells
- Replace `AFBitValue` with `std::bitset`
- Add `random` functions
- Replace `easylogging++` with `spdlog`
- Add BusPlugin(use `busid` as server id and communicate, for now not totally finished)
- Update `Proto` library to `ProtoPlugin`
- Update `Net` library to `NetPlugin`
- Update `brynet` to the newest version
- Refactoring the net plugin
- Add DataTable callback when `DataTable.Add/Set/Del(data)`
- Modify table row index
- Modify module register order and fix config's bug
- Optimise - Don't load DataNode in AFConfigModule when the node value is the default
- Add AFCMsgModule(not finished yet)

### version 0.8

- Replace `evpp` with `brynet` for reducing dependence
- Add `AFCronScheduleModule`, using `cron` syntax
- Add `WebSocket` support
- Replace `easylogging++` with `spdlog`
- Add `SonarCloud` check
- Add `Codecov` test
- Fix some warnings
- Fix some code smells of SonarCloud
- Fix GUID bug
- Fix some known bugs

### version 0.7

- Restructure log module
- Update to VS2017 as default IDE in windows
- Add function  `strlcpy`
- Use a new memory pool
- Fix `AFTimer` bugs
- Reduce `.git` disk usage
- Fix Net bugs
- Fix parameters warnings
- Fix tools, config id can be a number
- Rename Example files

### version 0.6

- Change Mem allocation
- Add Timer Module(Using time wheel)
- Modify Plugin DLL entry and exit functions by using macro `ARK_DLL_PLUGIN_ENTRY(Plugin Class)`
- Add `ARK_STATIC_ASSERT`
- Add launch parameters for launching multi-processes
- Use `std::is_base_of` to instead self derived class check

### version 0.5

- Fix some memory bugs
- Add memory pool and allocation
- Add wiki(Unfinished)
- Restructure file directories
- Add a new file processing tool

### version 0.4

- Unified concept, Property -> DataNode Record -> DataTable
- Unified configurations
- Update Google.Protobuffer 3.x
- Fix some bugs

### version 0.3

- Fix msg head bugs
- Rename some files

### version 0.2

- Modify base data structure
- Modify the performance of the network
- Modify directories structure
- Add CMake support(Linux platform is testing)
- Modify the compiling way of **Dep**
- Modify GUID to 128 bits

### version 0.1

- First release