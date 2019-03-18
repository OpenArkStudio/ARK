# 新的数据结构设计

## 多层数据结构

AFIData不再是仅有的几种数据结构，支持嵌套复杂结构

## 配置结构修改

核心配置分为两份

- EntityClass.meta 只有实例对象在里面
- ConfigClass.meta && Config.config 配置表，表头为meta, 内容为config