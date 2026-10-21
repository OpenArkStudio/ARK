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
因此，滚动更新的镜像可以共享归档容器，但容器恢复成功不代表其中每个包都兼容。

配置成功后，未命中或通过前缀恢复的运行只按精确 key 保存一次，不另外按兼容前缀保存第二份。
各镜像的快照仍可能包含重复归档并占用缓存空间。v1 key 无法通过这个前缀安全查找，因此不参与回退；
在可访问的运行保存匹配的 v2 缓存族之前，v2 仍需冷构建。
Actions 缓存的分支／引用可见性规则继续生效，详见 [Actions 缓存规则](https://docs.github.com/en/actions/reference/workflows-and-actions/dependency-caching#cache-key-matching)。
本次改动不能让另一个合并队列引用的缓存变得可读。
相互无法读取缓存的队列运行需要默认分支中兼容的 v2 归档作为初始来源。

身份信息缺失、依赖布局或归档路径不受支持时，继续关闭持久缓存并导出 `VCPKG_BINARY_SOURCES=clear`。
缓存服务失败不影响正常的源码配置、编译和回归检查。缓存范围仅为包归档，不包含依赖安装目录或项目目标文件。
本地 Node fixtures 验证查找边界、输出与工作流连接；托管环境中的缓存复用和耗时变化仍需独立 CI 证据。
