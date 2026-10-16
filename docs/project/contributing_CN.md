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
