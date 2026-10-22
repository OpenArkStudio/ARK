# 贡献与文档开发

改动通过 PR 提交到受保护的 main 分支。完成对应检查后，以英文提交标题 squash 合并。详见[贡献指南](https://github.com/OpenArkStudio/ark/blob/main/.github/CONTRIBUTING.md)。

## 开发文档

文档站使用 Cloudflare Nimbus 与 Astro。`docs/` 中的 Markdown 是内容源，每页包含英文文件及对应的 `_CN.md` 中文文件。生成的内容和站点输出不提交到仓库。

```sh
npm ci --prefix docs/site
npm run dev --prefix docs/site -- --host 127.0.0.1
npm run check --prefix docs/site
```

站点提供语言切换、搜索、代码高亮，以及供 AI 阅读的纯 Markdown 接口。PR 只运行文档检查，不发布站点。变更合并到 `main` 后，文档工作流构建并发布 GitHub Pages；维护者也可手动触发。文档构建需要 Node.js 22.12+；编译 ark 与 C++ 测试不需要 Node.js。


## CI 依赖包归档缓存

CI 使用 `actions/cache` 恢复执行机临时目录中的 vcpkg 包归档。
带版本号的 v2 key 由兼容前缀和精确执行机镜像后缀组成。
前缀包含执行机操作系统与架构、`ImageOS`、固定的 vcpkg 版本、manifest 和配置文件内容
（含 baseline）、本地 port overlay、选中和默认 feature、构建 profile、编译器环境参数，
以及 target/host triplet 覆盖项。只有 `ImageVersion` 移到后缀。
在 Actions 搜索的每个引用内，先查找精确 key，再使用这一个前缀，不提供仅按操作系统或跨 profile 的回退。
两个镜像字段仍为必填项。

恢复归档只为 vcpkg 的 `files` provider 提供候选包，不直接恢复依赖安装目录。
vcpkg 为每个请求的包计算完整 ABI hash，其中保留编译器跟踪；只有 hash 匹配时才取用归档，
不匹配的包仍从源码构建。工作流不会关闭编译器跟踪、覆盖 ABI hash 或重命名包归档。
具体规则见 [vcpkg ABI 不匹配说明](https://learn.microsoft.com/en-us/vcpkg/users/binarycaching-troubleshooting#cause-3-differences-between-local-and-remote-build-environments)。
因此，滚动更新的镜像可以共享归档缓存，但恢复成功不代表其中每个包都兼容。

配置成功后，未命中或通过前缀恢复的运行只按精确 key 保存一次，不另外按兼容前缀保存第二份。
各镜像的快照仍可能包含重复归档并占用缓存空间。v1 key 无法通过这个前缀安全查找，因此不参与回退；
可访问的运行尚未保存相同依赖配置的 v2 缓存时，v2 仍需冷构建。
Actions 缓存的分支／引用可见性规则继续生效，详见 [Actions 缓存规则](https://docs.github.com/en/actions/reference/workflows-and-actions/dependency-caching#cache-key-matching)。
本次改动不能让另一个合并队列引用的缓存变得可读。
相互无法读取缓存的队列运行需要默认分支中兼容的 v2 归档作为初始来源。

身份信息缺失、依赖布局或归档路径不受支持时，继续关闭持久缓存并导出 `VCPKG_BINARY_SOURCES=clear`。
缓存服务失败不影响正常的源码配置、编译和回归检查。缓存范围仅为包归档，不包含依赖安装目录或项目目标文件。
本地 Node 测试验证查找边界、输出与工作流连接；托管环境中的缓存复用和耗时变化仍需独立 CI 证据。


### Windows ABI 诊断

依赖配置及原有的归档保存步骤之后，Windows Applications 为 Asio、rapidxml 和 Protobuf
记录有界的 JSON 诊断。脚本只读取
`ci-build/vcpkg_installed/x64-windows/share/<port>/vcpkg_abi_info.txt`，
不搜索目录，也不调用构建工具。符号链接、非普通文件、格式错误、triplet 不符或超过 64 KiB 的文件均被拒绝。
元数据缺失或被拒绝时明确报告 `unavailable`；可选字段缺失时同样报告 `unavailable`，不将缺失视为相等。

`packageAbi` 是 ABI 元数据原始字节的 SHA256。字段值仅以 SHA256 指纹输出，字段名固定为：
`triplet`、`triplet_abi`、`features`、`cmake`、`powershell`、`compiler_hash`、`portfile.cmake`、
`ports.cmake`、`vcpkg.json`、`post_build_checks`、`sbom_info`、`vcpkg-cmake`、
`vcpkg-cmake-config`、`abseil` 和 `utf8-range`。
未知字段名和所有原始值均不输出，不记录完整环境、绝对路径或凭据，也不上传元数据文件或 artifact。
独立的 `compiler_hash` 字段可能不存在；诊断不会根据编译器版本字符串推断其值，也不拆解不透明的 `triplet_abi` 值。

此前步骤失败后，诊断仍会执行，任务取消时除外。状态只表示元数据是否可用，不能让配置、编译或测试的失败变为成功。
诊断错误也不会成为新增的原生验收条件。缓存 key/provider、编译器跟踪和构建设置均保持不变。
对比不同运行的指纹可以缩小 ABI 差异范围；已输出字段相同不能证明未输出的输入相同。
诊断不判定缓存命中，也不构成性能测量。此前分析的两轮 Windows 日志中，74 个包的 ABI 均发生变化；
导致变化的具体输入仍未得到证实。
