# 对象指针容器与所有权

包含 `<ark/container/vector.hpp>`，通过 CMake 的 `ark::core` 目标引入使用要求。
`pointer_vector<T>` 独占所存对象，`shared_vector<T>` 存储 `std::shared_ptr<T>`。
`value_vector<T>` 仍是 `std::vector<T>` 的别名。

## 所有权

| 操作 | `pointer_vector<T>` | `shared_vector<T>` |
| --- | --- | --- |
| 复制 | 禁止复制；转移所有权时使用移动 | 复制共享引用 |
| 移动 | 转移所有权，源容器变空 | 转移所存引用，源容器变空 |
| 替换槽位 | 删除原对象；同一槽位写入同一指针时保持不变 | 释放原共享引用 |
| `erase(index)` | 删除对象，保留空槽位 | 释放共享引用，保留空槽位 |
| `clear()` 或析构 | 删除全部拥有的对象 | 释放全部所存引用 |

自移动赋值保持容器不变。

每个非空裸指针只能由一个槽位拥有。不要将它放入其他槽位或拥有型容器，也不要手动删除通过
`at()` 或迭代器借用的指针。替换或清空槽位应使用容器方法；通过迭代器直接给裸指针槽位赋值，
会绕过容器的所有权处理。

插入成功后，所有权才转移给容器。追加槽位因内存分配失败而抛出异常时，传入的裸指针仍由调用方拥有。
可以用 `unique_ptr` 暂存对象，待插入返回后再释放其所有权：

```cpp
#include <memory>
#include <utility>

#include <ark/container/vector.hpp>

struct item
{
    int id;
};

int main() {
    ark::pointer_vector<item> objects;
    auto pending = std::make_unique<item>(item{7});
    objects.push_back(pending.get());
    pending.release();

    auto transferred = std::move(objects);
    transferred.erase(0); // 删除 item 7，保留空槽位。

    auto external = std::make_shared<item>(item{8});
    ark::shared_vector<item> shared;
    shared.push_back(external);
    auto copy = shared;
    shared.clear();
    copy.clear(); // external 仍然拥有 item 8。
}
```

## 槽位与迭代

`size()` 包含空槽位。`resize(n)` 只扩容，新槽位初始化为空指针；传入更小的值不会缩容。
`push_back(value)` 填入第一个空槽位，没有空槽位时追加。
`push_back(index, value)` 替换已有槽位；索引不小于 `size()` 时只追加一个槽位。
需要写入特定索引时，应先用 `resize()` 扩容。

`at(index)` 越界时返回空指针。`operator[]` 和 `erase(index)` 要求索引有效。
正向与反向迭代都会遍历空槽位。只读访问槽位时使用 `cbegin()` / `cend()` 或
`crbegin()` / `crend()`。并发访问容器时，由调用方负责同步。

## 迁移

转移独占所有权时，将 `pointer_vector` 的复制改为 `std::move`。
多个容器需要共享对象时使用 `shared_vector`，其元素与迭代器值类型为 `std::shared_ptr<T>`；
裸指针版本存储 `T*`。按值存储对象时，继续使用 `value_vector` 或 `std::vector`。
