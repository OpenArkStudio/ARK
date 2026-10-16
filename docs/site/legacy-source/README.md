# Original Docsify source archive

Snapshot: `d2b1f31ee7ac77aa8c273a38ed7c60c0b534084d`

This directory preserves the 40 articles identified by `scripts/migration-source.mjs`. Each file was exported as raw bytes from that Git snapshot and its original path; no translation, newline normalization, typo correction or link rewriting was applied. The snapshot identifier is provenance, not a claim about a remote repository or a new license. Any notices in the original files remain unchanged.

The archive-local `.gitattributes` disables line-ending conversion for Markdown, so checkout settings cannot rewrite the stored source bytes. Many originals intentionally have no final newline.

These are historical source records, not current API documentation. The three design sources `zh-cn/new-desgin.md`, `design/data.md` and `design/network.md` are Chinese; their English modern guides are translations/explanations, not invented historical English files. Original relative image/link paths are retained literally. Use the modern guides for current diagrams and links.

The site content preparer skips the entire `docs/site/` directory, so this archive is not included in current navigation, search or AI Markdown/llms exports. Open the source files in Git when tracing historical wording.

Run `node --test scripts/legacy-source.test.mjs` from `docs/site/`, or run the normal `npm run check`. The test verifies the snapshot identifier, every mapping and every file digest without requiring the old Git object in CI.

原文按下表路径与快照逐字节保留，不进行翻译、换行转换或链接改写。归档不表示当前功能，不进入现代站点导航与 AI 文档。三份设计原文只有中文，现代英文说明不伪造历史来源。原文件中的声明保持原样，本文不推断远端仓库或新增许可证。

## Source mapping and checksums

| Original path | Current content path | SHA256 |
| --- | --- | --- |
| `design/data.md` | `docs/project/data-model-proposal_CN.md` | `714d3233a81640f7077ffedf812323df18b9e35e1ce16c5ce410ff0cc66cf30f` |
| `design/network.md` | `docs/project/network-design_CN.md` | `373eb3e87967ec0b591a70995afe35e28383b8d40d31c94fb6c76ce2ade520e6` |
| `en-us/app-architecture.md` | `docs/concepts/application-architecture.md` | `951713661ce183c7ce1042d29603537ce5c080774d80de56fa3e7b9ed757abba` |
| `en-us/app.md` | `docs/guides/applications.md` | `6a4a8905456c1212f2296045c31e72805b85c144015075c2a2bb5c903f0d5b7b` |
| `en-us/benchmark.md` | `docs/performance/benchmark.md` | `4c3a4ac70a96043e9588760300309244adea1c3062d2ca439907dfc43d2287ab` |
| `en-us/bus-plugin.md` | `docs/reference/bus.md` | `279e1e9868085f2a23dbb3c1a72d63eac47fca1da86cea613b7a306c37f47477` |
| `en-us/changelog.md` | `docs/project/changelog.md` | `20ec0d28f438fa88b118cc484b2f13d1a8effb34b9021e9b390644246ed3ed3a` |
| `en-us/concept.md` | `docs/concepts/glossary.md` | `d6c2d9e75249a52c3b1a3b3e12f5006ddef68e4a51c72d081a0cc9790869475a` |
| `en-us/create-new-module.md` | `docs/guides/modules.md` | `d4c8b846c716924246c5e972d838415b3e57b966a99ef5df80562dffdef4100d` |
| `en-us/create-new-plugin.md` | `docs/guides/plugins.md` | `d9fb354f3b891bd6dad4f8189c4b19533897688f32084ea479b87e17f3df36a8` |
| `en-us/data-driven.md` | `docs/concepts/data-driven.md` | `457148d84344669a675e6f2d9ca647f0690c465c83042e899fc1dc8ca0da22bb` |
| `en-us/data-structure.md` | `docs/guides/data-structures.md` | `4a127c922a721ce05242f6748ee6d753d92711cf16ab9c79ee9e524b52adb259` |
| `en-us/directory-structure.md` | `docs/reference/directory-structure.md` | `c89e8ba1f08b230b143ad367f8aa7f3fd2a638d13ba3a554dc9dbc4999d39b9a` |
| `en-us/faq.md` | `docs/project/faq.md` | `3a80b1c867cf3794e677302468fa61b4fc77d3730927bbe97d66caaee7c61f98` |
| `en-us/kernel-plugin.md` | `docs/reference/kernel.md` | `c17d35d793fa4d04ea1bbadd9188e678903a4996301b9bf4eee8b720aff1eebc` |
| `en-us/net-plugin.md` | `docs/reference/network.md` | `0fe6d755e093cb7bd32ad9a15d3189e81be2a68d83004e0d27364b0587e7f98e` |
| `en-us/plan.md` | `docs/project/roadmap.md` | `615e7d945ee81faf21ee62426f7b485df30002d849fd97a5681bf24f2003404a` |
| `en-us/quick-start.md` | `docs/get-started/quick-start.md` | `8abb9f2e0ed4b30d40c716e934b1ffb4431455f0eca3de27d8da7f0fea4ee22a` |
| `en-us/server-architecture.md` | `docs/concepts/server-architecture.md` | `2677fa2414719c5ca003ab5eda8ba45f655bc3ebe7ca9abfe6c5e0c560123e49` |
| `en-us/tools.md` | `docs/guides/tools.md` | `6ae8c3fff68753ff29faf59997d3846fea749eedb356ad1266e8ab83dbca7927` |
| `zh-cn/app-architecture.md` | `docs/concepts/application-architecture_CN.md` | `d470fd8e545f9f05c38602430d432f1721dfd6aa6089faaa39ab20473a0100df` |
| `zh-cn/app.md` | `docs/guides/applications_CN.md` | `20d89185abe66b752b9e564413676eea6d6a8f9e42f16b0139d962bed50fc3ba` |
| `zh-cn/benchmark.md` | `docs/performance/benchmark_CN.md` | `b6e7b8e2655171a9ed9f0718f2460034311427bdb1fa3e143b134352b0be2114` |
| `zh-cn/bus-plugin.md` | `docs/reference/bus_CN.md` | `d72a1308328503cfe51d268004956819d625ec62c8344d08abeb100bf4b18c98` |
| `zh-cn/changelog.md` | `docs/project/changelog_CN.md` | `f1a3786c05515ec017843dff468cf367b2cdbe6a6f2cb7d93692385f6ab7ddb8` |
| `zh-cn/coding-guide.md` | `docs/project/code-style_CN.md` | `0dfb9817311b67791cd1b8edf418e42ffdd279630247ec1ceeb4946bb7cd9e7d` |
| `zh-cn/concept.md` | `docs/concepts/glossary_CN.md` | `d6c2d9e75249a52c3b1a3b3e12f5006ddef68e4a51c72d081a0cc9790869475a` |
| `zh-cn/create-new-module.md` | `docs/guides/modules_CN.md` | `6ced0eb07830db520e440ecee10219939202b876b148e68849f1d49723d7624a` |
| `zh-cn/create-new-plugin.md` | `docs/guides/plugins_CN.md` | `43999ed09f4f672ef03271f4ee29b7a2625f0f9b8402894bcedb9e2c3d810139` |
| `zh-cn/data-driven.md` | `docs/concepts/data-driven_CN.md` | `dfb088dbea3e981960d08e3e897392a6aa7dd4473be02a90db980670de6b5d2f` |
| `zh-cn/data-structure.md` | `docs/guides/data-structures_CN.md` | `4a127c922a721ce05242f6748ee6d753d92711cf16ab9c79ee9e524b52adb259` |
| `zh-cn/directory-structure.md` | `docs/reference/directory-structure_CN.md` | `1b0985c1486fc1a23d36f1350fcf467e31b6f0324d551fd82c19d3d3975cc10a` |
| `zh-cn/faq.md` | `docs/project/faq_CN.md` | `3f42a7a5f05c759484dba76bc08c4b04f7fae82d8fb66a64ff81486aacfc8b36` |
| `zh-cn/kernel-plugin.md` | `docs/reference/kernel_CN.md` | `c17d35d793fa4d04ea1bbadd9188e678903a4996301b9bf4eee8b720aff1eebc` |
| `zh-cn/net-plugin.md` | `docs/reference/network_CN.md` | `f34464371c3332d4410d6440ee25163851e5a22ac67e0763cb3af544e01c4754` |
| `zh-cn/new-desgin.md` | `docs/project/data-design_CN.md` | `5fff2197187e4224d4e32b13813d28a05f5fc6addcd2e6773baa5827567c024a` |
| `zh-cn/plan.md` | `docs/project/roadmap_CN.md` | `791bb943d2d173ab8f5942d8acb21b69fcdc3a57cf145ba4df03388cc639a146` |
| `zh-cn/quick-start.md` | `docs/get-started/quick-start_CN.md` | `df26605b857aaf8a73a5a0f72b036e0742242f874463ea6340dd50037890cde2` |
| `zh-cn/server-architecture.md` | `docs/concepts/server-architecture_CN.md` | `a84ebd78455a5f2844dfc16a409fc655f15755aa99fd9c0337a89398d3f4d63a` |
| `zh-cn/tools.md` | `docs/guides/tools_CN.md` | `b0757f3df92fcff965083dcf420eab9c37142e551618869a98032980ff0bb3b3` |
