# 部署示例

[English](README.md)

本目录集中保存原有容器构建文件与 Kubernetes 示例。YAML 中的服务、镜像、命名空间、
资源保持原样；仓库内插件的动态库名称与当前构建名称对齐。Photon 示例使用
`/home/server/escort` 作为工作目录，并将配置挂载到其中的 `conf/`，与应用镜像的运行布局一致。外部业务插件名称
保持原样。使用前需要检查目标环境；目录迁移不代表部署验收通过。

从仓库根目录执行镜像构建命令，使 `COPY .` 继续以源码仓库作为构建上下文：

```sh
docker build -t docker/ark-alpine-build:latest -f deployment/docker/ark-alpine-build .
docker build -t docker/ark-alpine-runtime:latest -f deployment/docker/ark-alpine-runtime .
docker build -t ark:escort -f deployment/Dockerfile .
```

应用 Dockerfile 的两个阶段仍使用构建镜像。单独的 runtime 构建文件作为原有选项保留，
应用构建文件没有选用它。这些构建文件保留了原有依赖与镜像选择，本次未通过容器引擎验证。

集群示例路径改为 `deployment/ark_consul.yaml`、`deployment/photon_config.yaml`、
`deployment/photon_game_deployment.yaml` 和 `deployment/photon_master_deployment.yaml`。
外部构建命令中的根目录 `Dockerfile`、`docker/` 和 `kubernetes/` 路径需要同步更新。
运行脚本说明见[应用指南](../docs/guides/applications_CN.md#启动一个应用)。

源码样例位于 `config/` 和 `resources/`；应用镜像将其部署为运行时的 `conf/` 和
`resource/`，运行时目录名称不变。部署使用 Kernel 的服务前，先生成并检查所需元数据。
保留的 Photon 镜像和外部业务插件仍需在目标环境中单独验证。
