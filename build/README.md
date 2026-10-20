# Runtime files

[简体中文](README_CN.md)

Build artifacts belong to `<cmake-build-directory>/bin`. Keep Release, Debug and
sanitizer builds in separate directories. Stage an application's executable and
plugins from the same build into a runtime directory containing `bin/`, `conf/`
and, when required, `resource/`. This source directory contains example
configuration, resources and tools; generated binaries are not checked in.

## Run one application

The shell and Windows helpers share one interface:

```text
run.sh  <Release|Debug> <name> <busid> [runtime-directory]
run.bat <Release|Debug> <name> <busid> [runtime-directory]
```

The default runtime directory is the script's directory. An explicit runtime path
is relative to the caller's working directory. The helper selects `bin/app`
(`app.exe` on Windows) for Release or `bin/app_d` (`app_d.exe`) for Debug, reads
`conf/<name>.app.conf` and `conf/log.conf`, and runs in the foreground. The name
also becomes the application's `--name` value. Use the executable directly for
TOML manifests, a different process name or other launcher flags.

After preparing the [Sample1 runtime](../docs/get-started/quick-start.md), run from
the repository root:

```sh
sh build/run.sh Release sample1 1.0.1.1 run/sample1
```

On Windows, use `build\run.bat Release sample1 1.0.1.1 run\sample1`.
Check the application's initialization log before using it; the helper's path
checks do not validate plugins, business configuration or readiness.

## Stop one application

Prefer Ctrl+C on macOS/Linux or `exit` in the Windows application console.
For a process whose PID has been confirmed, `kill.sh <pid>` requests `SIGTERM`;
`kill.bat <pid>` forcibly terminates that PID. Both helpers reject other executable
names and require exactly one explicit PID. Recheck the PID's executable and
command line immediately before use; a PID can be reused after a process exits.
There is no process-name wildcard or machine-wide stop command.

## Migrate old helper commands

- Replace `rund.sh` / `rund.bat` with `run.sh Debug ...` / `run.bat Debug ...`.
- Replace `sample1.sh`, `mysql_sample.sh` and `sample5http_test.sh` with
  `run.sh Debug <name> <busid>`, using `sample1`, `mysql_sample` or `sample5`.
- The old no-argument five-service launch sequence used missing `plugin_conf/`
  paths and inconsistent Release/Debug Bus IDs. Supply each application's name
  and Bus ID explicitly, and arrange service startup separately for the target
  environment. The helper no longer daemonizes applications.
- Replace `vc_clear.bat` with `cmake --build <cmake-build-directory> --target clean`
  to clean a selected build directory.
- Container recipes and cluster examples now live under [deployment/](../deployment/README.md).

The byte-preserved historical articles under `docs/site/legacy-source/` still show
their original `bin/rund.*` commands. Use the current commands above instead.
