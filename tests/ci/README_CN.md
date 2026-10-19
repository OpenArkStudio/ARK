# CI 策略测试

使用原生 Node 测试入口，不需要安装 npm 包：

```sh
node --test tests/ci/*.test.mjs
```

测试使用临时的本地 Git 仓库，不需要 GitHub API token、网络服务、C++ 重编译或数据库。
覆盖包含重命名旧路径的完整文件集合、按事件选择模块、文档站可用性，以及任务结果的严格校验。

源分支 PR 复用现有配置、编译和回归步骤，在 Linux Boost 上执行相关模块。
Core/Application 测试选择 Applications；网络测试选择 Network；数据库测试选择 Database。
Network、HTTP、Bus 插件变更还检查 Applications；MySQL、Redis 和共享数据库支持代码变更
检查 Applications 与 Database。公共核心头文件、共享构建或依赖、CI 和未知路径变更
检查全部三个源分支模块；枚举缺失、无效或为空时回退到全部 12 个任务的完整矩阵。
原生代码检查同时要求 clang-format 18 通过。文档和 README 链接检查沿用现有路径规则；
源分支 CI 成功只表示模块验证通过。

Merge Queue 候选直接比较事件中的 `base_sha` 与 `head_sha`，包含前序排队 PR 带来的变更。
候选包含代码、构建、依赖、CI 或未知路径时，执行全部 12 个原生任务：Network 的 Linux、macOS、
Windows 与 Linux ASan/UBSan、TSan；Applications 的 Linux、macOS、Windows；Database 的
Linux Boost、Linux Standalone、macOS Boost、Windows Boost。手动运行保留相同完整矩阵。
只有范围已知、非空、文档站可用的纯文档变更才跳过原生任务。文档站存在时，合并候选仍检查文档；
README 变更还需通过其链接检查。

required gate 核对当前工作流的真实事件和声明的验证阶段，然后要求选中的任务成功、
不需要的任务明确跳过。失败、取消、未知或缺失的结果均不通过；源分支结果不能替代队列结果。
最终验收仍要求包含前序变更的合并候选通过检查。受保护的 `main` 只接收已通过必需检查的候选；
合入后的推送不再对同一代码树重复执行 CI。独立的文档工作流仍在 main 推送后发布文档。
README 徽章展示合并候选的工作流状态。

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
