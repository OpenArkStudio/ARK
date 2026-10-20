# Deployment examples

[简体中文](README_CN.md)

This directory groups the existing container recipes and Kubernetes examples.
The YAML files retain their existing services, images, namespaces, settings and
resources; their in-tree plugin library stems follow the current build names.
External business-plugin names remain unchanged. Review these environment-specific
examples before use; moving them does not establish working deployment acceptance.

Run image build commands from the repository root so `COPY .` keeps the source
checkout as its build context:

```sh
docker build -t docker/ark-alpine-build:latest -f deployment/docker/ark-alpine-build .
docker build -t docker/ark-alpine-runtime:latest -f deployment/docker/ark-alpine-runtime .
docker build -t ark:escort -f deployment/Dockerfile .
```

The application Dockerfile still uses the build image for both stages. The
separate runtime recipe is retained as an existing alternative; it is not selected
by the application recipe. These recipes retain their pre-existing dependency
and image choices and have not been validated against a container engine here.

Cluster example paths are now `deployment/ark_consul.yaml`,
`deployment/photon_config.yaml`, `deployment/photon_game_deployment.yaml` and
`deployment/photon_master_deployment.yaml`. Update external build commands that
used the root `Dockerfile`, `docker/` or `kubernetes/` paths. Runtime launch helpers
are documented in [build/README.md](../build/README.md).
