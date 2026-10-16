# CI 策略测试

使用原生 Node 测试入口，不需要安装 npm 包：

```sh
node --test tests/ci/*.test.mjs
```

测试使用临时的本地 Git 仓库，不需要 GitHub API token、网络服务、C++ 重编译或数据库。
覆盖包含重命名旧路径的完整文件集合、无法确定范围时执行完整 C++ 矩阵、文档站可用性，
以及各任务结果的严格校验。推送 main 和手动运行始终执行完整 C++ 矩阵。

只有根 README 变更时才检查其本地文件链接，包含图片、HTML `srcset` 和引用式链接。
检查器覆盖这些 README 使用的表达形式，不是完整的 Markdown 解析器；不验证远程 URL 或标题锚点。
Nimbus 检查单独验证生成文档站内的链接与锚点，不包含根 README 的标题锚点。
