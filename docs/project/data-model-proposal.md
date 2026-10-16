# Nested data and configuration proposal

<Aside type="note" title="Draft">

This historical proposal explores nested values and separate entity/configuration definitions. It is not a runtime API, an accepted business-resource format, or an automatic schema-upgrade feature.

</Aside>

## Intended direction

The original note wanted the data abstraction to represent nested structures rather than only a fixed set of basic value forms. For example, a business record could contain a structure, an array or a keyed collection of other values. This describes a modeling goal; it does not define wire encoding, persistence layout or C++ ownership.

![Proposed separation of metadata and nested values](../assets/draft-data-model.svg)

The note also separated runtime entity definitions from reusable configuration definitions and their values:

| Historical filename | Proposed meaning |
| --- | --- |
| `EntityClass.meta` | Definitions for runtime instance objects |
| `ConfigClass.meta` | Configuration record schema, analogous to table columns |
| `Config.config` | Configuration records using that schema |

These are names in the old proposal. They are not new files required by the current loader. Current resource lists are `resource/meta/entity_class.config` and `resource/meta/config_class.config`; see [data-driven loading](../concepts/data-driven.md).

## Current interfaces and the proposed extension

Current Kernel supports typed entity nodes, schema-backed table rows and containers of child entities. Use those interfaces for present applications. A container of entities is not automatically a general array of arbitrary nested values, and a Protobuf conversion function does not define migration for a future value hierarchy.

A future extension would need to specify type validation, nested references, ownership and removal behavior, change notification, serialization and compatibility. It must also say how class/field indices and existing configuration records map to the new structures. The proposal does not yet supply those rules or a safe reload protocol.

## Migration boundary

Keep existing metadata names, generated indices, saved data and external message contracts consistent until a migration has been explicitly designed and tested. Service-process configuration and business data are separate concerns; choosing a service manifest format does not select the representation of these entity/configuration resources.

Use [data structures](../guides/data-structures.md) for today's model and [the related design note](data-design.md) for the metadata sketch. The Chinese [original source](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/design/data.md) is preserved in the [source archive](https://github.com/OpenArkStudio/ark/blob/main/docs/site/legacy-source/README.md); no historical English source is claimed.
