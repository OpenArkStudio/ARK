# CI 策略测试

使用原生 Node 测试入口，不需要安装 npm 包；Windows 运行时路径导出的跨平台测试需要 CMake 3.21 或更新版本：

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
Windows，以及 Linux AddressSanitizer（内存错误检测）与 UndefinedBehaviorSanitizer（未定义行为检测）组合、
ThreadSanitizer（线程数据竞争检测）；Applications 的 Linux、macOS、Windows；Database 的
Linux Boost、Linux Standalone、macOS Boost、Windows Boost。手动运行保留相同完整矩阵。
只有范围已知、非空、文档站可用的纯文档变更才跳过原生任务。文档站存在时，合并候选仍检查文档；
README 变更还需通过其链接检查。

文档、根 README 链接和 C++ 格式检查合并到一个 `validation` job，共用一次 checkout 和 Node 设置。
文档站与锁文件可用时，Nimbus 复用 npm 包缓存；必需的文档站不可用时，预检仍会失败。
只有命令不存在时才安装 clang-format 18。各项检查保留原有范围标志，并导出实际步骤的 `outcome`；
前一项检查失败后，后续被选中的检查仍会执行。范围分类保留独立 job，分类结束后，原生矩阵与轻量验证可同时启动。
工作流定义由 8 个 job 减为 6 个；全部 12 个原生变体和所有轻量检查均被选中时，执行机任务由 17 个减为 15 个。
实际等待时间取决于执行机可用性和检查耗时。

required gate 核对当前工作流的真实事件和声明的验证阶段。选中的原生任务与合并后的验证任务必须成功；
不需要的原生任务必须明确跳过。选中的验证步骤必须报告 `success`，不需要的步骤必须报告 `skipped`。
合并任务成功不能替代缺失或被跳过的必需步骤。失败、取消、未知或缺失的结果均不通过；源分支结果不能替代队列结果。
最终验收仍要求包含前序变更的合并候选通过检查。受保护的 `main` 只接收已通过必需检查的候选；
合入后的推送不再对同一代码树重复执行 CI。独立的文档工作流仍在 main 推送后发布文档。
README 徽章展示合并候选的工作流状态。

每个既有的 PR/引用并发分组最多执行一轮，只保留最新进入该分组的一轮等待任务。
新运行等待旧轮结束，不再自动取消正在执行的旧轮。每个已运行的 job 保留原有超时限制，
等待时间不计入这些限制；Merge Queue 候选使用各自的引用分组。required gate 仍拒绝失败、
取消或缺失的结果。手动取消仍可能使 `always()` 汇总无法完成。
详见 [GitHub 并发规则](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#concurrency)。

仅额外将两个精确的插件索引路径 `src/plugins/README.md` 与 `src/plugins/README_CN.md` 归为文档。
文档站可用时，只修改这两个索引的 PR 与纯文档合并候选会选择 Nimbus 检查。
这不会将根 README 链接检查扩展到插件索引；Nimbus 验证生成的文档站，不直接检查索引文件。
`src/` 下的其他 Markdown、邻近构建／脚本、混合源码和不确定范围仍沿用保守的原生检查策略。

只有根 README 变更时才检查其本地文件链接，包含图片、HTML `srcset` 和引用式链接。
检查器覆盖这些 README 使用的表达形式，不是完整的 Markdown 解析器；不验证远程 URL 或标题锚点。
Nimbus 检查单独验证生成文档站内的链接与锚点，不包含根 README 的标题锚点。

Windows Applications CI 通过 `VCPKG_APPLOCAL_DEPS=OFF` 关闭 vcpkg 对每个目标执行的本地依赖复制。
固定版本的工具链默认会在共享库和可执行文件构建后复制依赖，其中多个目标使用同一个输出目录。
配置完成后，`windows-runtime.cmake` 从真实的 `CMakeCache.txt` 读取依赖安装根目录、目标 triplet 与构建配置。
脚本要求对应的依赖 `bin` 目录存在，并通过 `GITHUB_PATH` 只导出该目录，使后续并行构建、回归目标与应用子进程继承此路径。
Debug 使用 `debug/bin`，发布类配置使用 `bin`，不回退到其他配置。缓存参数无效、运行时目录缺失或输出通道不可用时，任务明确失败。
Network、Database 和普通开发构建保留现有行为。测试实际执行 CMake 缓存加载、目录选择、子进程 PATH 继承与错误传递。
这一调整移除了共享输出目录中的本地依赖复制写入，但不能据此确认已观察到的 Windows 文件锁由哪个进程持有。
真实 DLL 加载和完整应用回归仍须由 Windows CI 验收。

vcpkg 缓存测试覆盖 Windows/Unix 归档路径、依赖和执行机镜像变化引起的 key 变化、
缓存配置导出、目录不可用，以及明确的无缓存构建路径。C++ CI 通过 Actions 缓存本地依赖包归档，
每个包的 ABI 仍由 vcpkg 校验。未命中缓存或缓存服务不可用时，依赖构建和测试照常执行；
归档在 CMake 配置成功后、ark 编译和测试之前保存；后续编译或测试失败仍会使任务失败。
工作流检查覆盖该顺序，并确保配置、编译和测试失败时不会忽略错误。缺少执行机镜像信息或归档路径不受支持时，
关闭持久缓存并导出 `VCPKG_BINARY_SOURCES=clear`，避免 run-vcpkg 再选择已移除的 `x-gha` 默认值。
缺少 GitHub 文件命令通道时明确失败。不缓存项目的 C++ 编译产物、依赖安装目录、凭据或编译器缓存。
