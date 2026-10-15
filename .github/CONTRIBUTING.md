# Contributing to ARK

Welcome to [report Issues](https://github.com/OpenArkStudio/ark/issues) or [pull requests](https://github.com/OpenArkStudio/ark/pulls). It's recommended to read the following Contributing Guide first before contributing.

## Issues

We use Github Issues to track public bugs and feature requests.

### Search Known Issues First

Please search the existing issues to see if any similar issue or feature request has already been filed. You should make sure your issue isn't redundant.

### Reporting New Issues

If you open an issue, the more information the better. Such as detailed description, screenshot or video of your problem, logcat or code blocks for your crash.

## Pull Requests

We strongly welcome your pull request to make `ARK` better.

### Branch Management

Create a branch from `main` and submit changes through a pull request targeting
`main`. The branch requires passing CI and resolved conversations; force pushes
and deletion are prohibited, including for administrators. Merge with squash
and use an English commit subject. A second-maintainer approval is currently
optional.

### Make Pull Requests

Pull requests must pass the `CI - required` check before merging.

Before submitting a pull request, please make sure the followings are done:

1. Fork the repo and create your branch from `main`.
2. Update code or documentation if you have changed APIs or important functions.
3. Add the copyright notice to the top of any new files you've added.
4. Check your code lints and check styles.
5. Test and test again your code.
6. Submit your pull request to `main`.

## Code Style Guide

Use the repository `.clang-format` with clang-format 18. Public types, functions, namespaces and filenames use `snake_case`; private members end in `_`. Order includes as system headers, third-party headers, then ark headers. Keep function opening braces on the declaration line, use explicit braces for control flow, and separate definitions with a blank line.

See [Project layout and SDK](../docs/project-layout.md) for public include paths and CMake targets.
See [Code style](../docs/code-style.md) for naming rules, Protobuf migration and local checks.

## License

By contributing to `ARK`, you agree that your contributions will be licensed
under [Apache LICENSE](https://github.com/OpenArkStudio/ark/blob/main/LICENSE)
