# 新的设计思路

## 数据管理

```plain text
                                        |- MetaEntityClassSetting
        |--- MetaClassSettingBase*  ----|
        |           ||                  |- MetaConfigClassSetting
        |           ||
        |           ||
        |          \||/                 |- MetaEntityDataSetting
        |           \/                  |
AFData -|--- MetaDataSettingBase*   ----|
        |                               |
        |                               |- MetaConfigDataSetting
        |
        |                               |- bool
        |--- Parent(AFData*) -----------|- enum
                                        |- numberic(uint8/int8/uint16/int16/uint32/int32/uint64/int64/double/float)
                                        |- string
                                        |- struct(Map<std::string, AFData*>)
                                        |- array(vector<AFData*>)
                                        |- table(map<uint64_t, AFData*>)
```

## 数据说明

### MetaEntityClassSetting

class_name
multi-MetaEntityDataSetting

### MetaConfigClassSetting

class_name
multi-MetaConfigDataSetting

### MetaEntityDataSetting

包含Type 和 Mask, 函数, 脚本等

### MetaConfigDataSetting

只有类型和refer, 没有mask, 没有其他函数等

## 参考

- Unreal4
- KFrame