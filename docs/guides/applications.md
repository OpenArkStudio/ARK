# Application launcher

The `app` executable selects a process identity, reads an XML application manifest and loads its plugin libraries. Start with the runnable [Sample1 quick start](../get-started/quick-start.md); this page explains configuration paths, library names and startup checks.

## Command-line arguments

| Argument | Required | Meaning |
| --- | --- | --- |
| `--busid` / `-b` | Yes | Logical process Bus address, such as `1.0.1.1`; this is not a socket bind address |
| `--name` / `-n` | Yes | Application name, also used in the log filename |
| `--app_conf` / `-a` | Yes | XML application manifest; use a `*.app.conf` filename |
| `--log_conf` / `-l` | No | Logger configuration; defaults to `./conf/log.conf` |
| `--help` / `-h` | No | Print the launcher's argument reference |

```sh
./bin/app --busid=1.0.1.1 --name=sample1 \
  --app_conf=conf/sample1.app.conf --log_conf=conf/log.conf
```

`busid`, `name` and `app_conf` must be supplied explicitly. The [Bus reference](../reference/bus.md) describes the address fields. Use the executable's `--help` output for additional platform-specific flags.

## Resolve configuration paths

Relative paths resolve from the process working directory. Moving the executable or placing the manifest in another folder does not change that base directory.

![Configuration paths resolved from the working directory](../assets/launcher-paths.svg)

| Input | How it is selected | Example from the runtime directory |
| --- | --- | --- |
| Application manifest | `--app_conf` | `conf/sample1.app.conf` |
| Logger settings | `--log_conf`, or its default | `conf/log.conf` |
| Plugin libraries | `plugin_lib_dir` plus the library filename | `bin/Sample1Plugin.dylib` on macOS Release |
| Per-plugin settings | `plugin_conf_dir` plus that plugin's `conf` value | `conf/my-plugin.conf` when `conf="my-plugin.conf"` |
| Kernel metadata and configuration | The Kernel's `resource/` directory | `resource/meta/entity_class.config`, `resource/server/item.config` |

The launcher passes the per-plugin path to the plugin manager; each module decides how to read its own configuration. Kernel resource paths are separate from the application manifest and are needed only by applications that use those Kernel facilities. See [Data-driven configuration](../concepts/data-driven.md) and [Configuration tools](tools.md).

## Select plugins with XML

The checked-in [Sample1 manifest](https://github.com/OpenArkStudio/ark/blob/main/build/conf/sample1.app.conf) loads a utility plugin and one sample plugin:

```xml
<xml>
    <plugins plugin_lib_dir="bin/" plugin_conf_dir="conf/">
        <plugin name="AFUtilityPlugin" conf="" />
        <plugin name="Sample1Plugin" conf="" />
    </plugins>
</xml>
```

- Keep the outer `xml` element: the launcher looks for its `plugins` child.
- Keep the trailing separator in directory prefixes such as `bin/` and `conf/`; the current loader concatenates strings.
- Use the library stem in `name`, without a platform suffix or a `lib` prefix. The loader appends `.dll`, `.so` or `.dylib`; Debug builds also append `_d` before the suffix.
- An empty `conf` value supplies no per-plugin filename. Sample1 does not need a separate `sample1.conf` file.

Copy the executable and selected plugin libraries from the same build. Keep Debug and Release artifacts in separate runtime directories. The XML list is not a dependency-order contract: modules should resolve their dependencies through the [module API](modules.md), rather than rely on entry order.

## Check startup and stop the process

For Sample1, look for a log entry containing `sample1_module` and `PostInit`. This identifies entry into the sample's post-initialization callback, which registers its timer. A `LoadPlugin` message alone only identifies a library the loader is attempting to use.

On macOS/Linux, Ctrl+C or `SIGTERM` requests shutdown through the application's main loop. On Windows, enter `exit` in the application console. Sample1 logs `PreShut` and then `Shut` during orderly shutdown. This sample check is not a readiness or shutdown guarantee for every plugin combination.

<Aside type="caution" title="Do not use process survival as the readiness check">

The current launcher can return exit code `0` for argument or manifest failures, and it does not aggregate every plugin/module startup result. Check the expected module's initialization log and application-specific readiness before routing traffic to a process. A running PID or a zero exit status alone is insufficient.

</Aside>

## Troubleshoot startup

| Symptom | Check |
| --- | --- |
| Usage text or `Application parameter is invalid` | Supply all three required arguments; check spelling with `--help` |
| Manifest or logger configuration cannot be read | Check the working directory and both explicit paths; keep the `*.app.conf` naming convention |
| `Load shared library[...] failed` | Check `plugin_lib_dir`, platform suffix, Debug suffix and the library's dependent shared libraries |
| Plugin loads but the expected module is unavailable | Check registration, enabled targets and required plugins; see [Create a plugin](plugins.md) |
| Kernel metadata or configuration is missing | Stage the generated `resource/meta/` and `resource/server/` files in the process working directory |

The launcher implementation is in [main.cpp](https://github.com/OpenArkStudio/ark/blob/main/src/app/main.cpp). Review existing multi-process scripts before adopting them: some retain older deployment paths and service assumptions.
