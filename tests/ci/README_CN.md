# CI 策略测试

使用原生 Node 测试入口，不需要安装 npm 包：

```sh
node --test tests/ci/*.test.mjs
```

测试使用临时的本地 Git 仓库，不需要 GitHub API token、网络服务、C++ 重编译或数据库。
覆盖包含重命名旧路径的完整文件集合、无法确定范围时执行完整 C++ 矩阵、文档站可用性，
以及各任务结果的严格校验。推送 main 和手动运行始终执行完整 C++ 矩阵。

每个既有的 PR/引用并发分组最多执行一轮，只保留最新进入该分组的一轮等待任务。
新运行等待旧轮结束，不再自动取消正在执行的旧轮。每个已运行的 job 保留原有超时限制，
等待时间不计入这些限制；Merge Queue 候选使用各自的引用分组。required gate 仍拒绝失败、
取消或缺失的结果。手动取消仍可能使 `always()` 汇总无法完成。
详见 [GitHub 并发规则](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#concurrency)。

只有根 README 变更时才检查其本地文件链接，包含图片、HTML `srcset` 和引用式链接。
检查器覆盖这些 README 使用的表达形式，不是完整的 Markdown 解析器；不验证远程 URL 或标题锚点。
Nimbus 检查单独验证生成文档站内的链接与锚点，不包含根 README 的标题锚点。

vcpkg 缓存测试覆盖 Windows/Unix 归档路径、依赖和执行机镜像变化引起的 key 变化、
缓存配置导出、目录不可用，以及明确的无缓存构建路径。C++ CI 通过 Actions 缓存本地依赖包归档，
每个包的 ABI 仍由 vcpkg 校验。未命中缓存或缓存服务不可用时，依赖构建和测试照常执行；
归档在 CMake 配置成功后、ark 编译和测试之前保存；后续编译或测试失败仍会使任务失败。
工作流检查覆盖该顺序，并确保配置、编译和测试失败时不会忽略错误。缺少执行机镜像信息或归档路径不受支持时，
关闭持久缓存并导出 `VCPKG_BINARY_SOURCES=clear`，避免 run-vcpkg 再选择已移除的 `x-gha` 默认值。
缺少 GitHub 文件命令通道时明确失败。不缓存项目的 C++ 编译产物、依赖安装目录、凭据或编译器缓存。
