# SDK consumer

This is an independent CMake project using `ark::core`, `ark::net`, `ark::kernel`,
`ark::mysql`, `ark::redis` and `ark::utility`. It checks public headers and transitive build-tree
usage requirements without building ark applications or database plugin libraries.
Run `cmake --build <build-dir> --target check-sdk` after configuration.

Database and Utility public headers are staged from explicit lists beside their implementations;
the consumer also checks Utility public/helper mappings and rejects visible implementation/assembly headers. Redis's interface is
checked in all builds; the MySQL compile check requires `ARKNET_ENABLE_SSL=ON`.
This is an `add_subdirectory` build-tree consumer, not an installed-package test.

Build instructions: [English](../../docs/reference/directory-structure.md) · [简体中文](../../docs/reference/directory-structure_CN.md).
