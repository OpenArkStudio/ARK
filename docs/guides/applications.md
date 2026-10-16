# Application launcher

The launcher loads application configuration and the selected plugin libraries. Its command-line parser is in [main.cpp](https://github.com/OpenArkStudio/ark/blob/main/src/app/main.cpp).

| Argument | Purpose |
| --- | --- |
| `--busid` / `-b` | Process Bus address |
| `--name` / `-n` | Application name |
| `--app_conf` / `-a` | Application configuration path |
| `--log_conf` / `-l` | Logging configuration path |
| `--help` / `-h` | Print usage |

Start with the provided [build/run scripts](https://github.com/OpenArkStudio/ark/tree/main/build) and [configuration](https://github.com/OpenArkStudio/ark/tree/main/build/conf). Choose the application, paths and process identity for the deployment before running those scripts.
