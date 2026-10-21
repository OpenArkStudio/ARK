# Legacy application-manifest fixtures

These XML files retain the effective input before the built-in TOML preset
migration. They come from the local integration of Bus configuration commit
`117052fa4c17876632ed6ae63c9995d31a387c1b` and database-preset commit
`0c3ffad23701e1a2218d83e38046601e2a926add`. The three `photon.*` inputs came from
`deployment/photon_config.yaml`; the other 24 came from `config/`.

The native manifest regression loads these originals through the legacy XML
reader and compares directory strings, plugin/configuration mappings and the
resulting map traversal with the shipped TOML presets and ConfigMap payloads.
Traversal equivalence is checked on the active standard library; neither format
makes declaration order a public lifecycle-order contract.

Keep the ignored `res` nodes, `world`'s ignored `path` attribute, absent `conf`
attributes and empty configuration values. They prove that migration preserves
the existing reader's effective schema. The invalid external Freebird sample is
excluded from migration and remains in `config/`; the XML suite still covers it.
