# Code style

This convention applies to the ark reorganization and subsequent development. Public interfaces, implementation names and filenames consistently use `snake_case`.

## Naming

| Item | Convention | Examples |
| --- | --- | --- |
| Namespaces | Lowercase words separated by underscores | `ark`, `ark::detail` |
| Classes, structs and type aliases | `snake_case` | `plugin_manager`, `module`, `data_list` |
| Enumeration types and values | `snake_case` | `data_type::dt_int64` |
| Functions and methods | `snake_case` | `find_module()`, `start()`, `stop()` |
| Local variables and parameters | `snake_case` | `module_name`, `initial_buffer_size` |
| Private and protected members | `snake_case` with a trailing underscore | `module_name_`, `state_` |
| Public struct fields | `snake_case` | `session_id` |
| Compile-time constants | `snake_case` | `default_buffer_size` |
| Template parameters | `snake_case` | `module_type`, `buffer_size` |
| Preprocessor macros | `UPPER_SNAKE_CASE`, prefixed with `ARK_` | `ARK_EXPORT` |
| C++ filenames | `snake_case`; `.hpp` headers and `.cpp` implementations | `plugin_manager.hpp`, `plugin_manager.cpp` |

The public type form after migration is `ark::plugin_manager`. Name interfaces and implementations by responsibility; new interfaces do not use `AFI` or `AFC` type prefixes. Preserve identifiers belonging to dependencies.

## Formatting and include order

- Use four spaces, no tabs, and a 120-column limit.
- Group includes in this order: system libraries, third-party libraries, project headers. Separate groups with a blank line.
- Place a function's opening brace on the signature line. Expand short functions, including empty bodies.
- Use braces for conditional and loop bodies, including short branches.
- Leave blank lines between includes and code, namespaces and their definitions, and adjacent type or function definitions.
- `clang-format` handles formatting. Interface review and applicable static checks enforce naming.

This example illustrates naming and formatting:

```cpp
#include <cstddef>

namespace ark {

class session_counter
{
public:
    void add(std::size_t count) {
        count_ += count;
    }

    std::size_t count() const {
        return count_;
    }

private:
    std::size_t count_ = 0;
};

} // namespace ark
```

## Implementation conventions

- Use C++20 and target-scoped dependencies. Public headers explicitly include the standard-library headers they need.
- Manage resources through RAII and explicit ownership. Document the lifetime of borrowed pointers, references and views.
- Use `override`, `final` and `const` to express interface constraints.
- Write English comments describing constraints, reasoning and non-obvious behavior. Document public API lifetime, threading and error contracts.
- Review formatting separately from behavior changes. Migrate a module together with its consumers, examples and tests.

## Local checks and generated code

Use clang-format 18 and the checked-in configuration:

```sh
./scripts/format.sh
./scripts/format.sh --check
./scripts/clang_tidy.sh style-build
```

The clang-tidy command needs the configured build's compilation database and a compatible compiler toolchain. Set `CLANG_FORMAT` or `CLANG_TIDY` when selecting a nondefault executable. Formatting covers ark-owned C++ and Protobuf sources; vendored implementations and generated build outputs retain their own rules.

Protocol definitions use `snake_case` and the `ark::protobuf` C++ namespace, with `consul` and `osslog` retaining their existing packages. The generator's `.pb.h` suffix remains unchanged. Configuration keys, reflection strings, external JSON names and the C# generator's business-facing names keep their required spelling. See [Protobuf messages](../reference/protobuf.md) for binary and source compatibility boundaries.
