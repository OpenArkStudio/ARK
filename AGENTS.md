# Contributor and agent instructions

These rules apply to this repository. Preserve unrelated work and use an isolated
branch or worktree when the existing checkout contains uncommitted changes.

## Commits and pull requests

- Write all new commit subjects and PR titles in English using Conventional
  Commits: `type(scope): description`. Use a short, lowercase scope and an
  imperative description of the resulting change.
- Types: `feat`, `fix`, `perf`, `refactor`, `docs`, `test`, `build`, `ci`, `style`,
  `chore` and `revert`. Choose the type that describes the actual change; use
  `style` only for formatting changes without behavior changes.
- Examples: `feat(config): support TOML application manifests`,
  `fix(logging): preserve the configured log directory`,
  `ci(cache): restore vcpkg package archives`.
- Name new topic branches `type/short-kebab-case`, matching the change, for example
  `feat/database-coroutines`, `fix/windows-includes` or `docs/readme-maintainers`.
  Do not use the `codex/` prefix or generic `xxx-branch` names. Existing published
  branches do not need renaming solely to adopt this rule. Use lowercase names
  for new GitHub repositories.
- Mark incompatible public changes with `!` and explain them in a
  `BREAKING CHANGE:` footer. Describe migration steps in the PR and release notes.
- Keep each PR focused on one functional module or related change. Publish
  independently reviewable work instead of accumulating unrelated modifications.
- All changes to `main` go through a PR. Do not push directly, force-push or delete
  `main`, bypass branch protection, or rewrite already merged history.
- Use squash only after every required CI check passes and review conversations
  are resolved. Outside the merge queue, synchronize the PR head with `main`
  before acceptance. With the merge queue enabled, validate the queue candidate
  containing the latest `main` and preceding queued changes; a green PR head alone
  does not validate that combination. Add eligible PRs to the queue promptly.
  Another maintainer's approval is not currently mandatory. Merge eligible PRs
  without asking for confirmation again.
- Explicitly set the squash commit subject to the Conventional Commit PR title.
  Replace GitHub's generated commit-list body with a concise change description;
  do not copy intermediate commit subjects into release notes.

## Verification and delivery

- When runner capacity is shared by several PRs, prioritize the oldest unblocked
  PRs and their required queue candidates. Prepare newer work locally and defer
  its hosted runs until older ready PRs can finish; do not repeatedly push newer
  branches ahead of the backlog. Preserve active acceptance runs. A cancelled
  scheduling run still needs fresh successful checks before acceptance.
- Run the checks relevant to the change before publishing. Report the exact
  tested revision, commands, results and material limitations.
- Investigate failed CI logs before changing code or retrying. Do not repeatedly
  rerun an unexplained failure, weaken tests or protection, or extend timeouts to
  hide a defect.
- Pending, skipped, cancelled and successful checks are distinct states. A
  cancelled run is not validation. If a run is cancelled for scheduling, record
  the reason and resume its acceptance checks; do not leave it indefinitely red.
- Use the repository's change-scope policy: documentation-only changes run the
  relevant documentation/link checks. Source PRs run the affected native modules
  on Linux Boost and clang-format 18; shared SDK, build, dependency, CI and unknown
  paths require every source module when the changed range is known. Unavailable,
  malformed or empty ranges fall back to the full matrix. Code merge-queue
  candidates and manual runs retain the full cross-platform and sanitizer matrix.
  Source checks admit a PR to the queue;
  they do not establish final matrix acceptance. Never fabricate a passing result.
- Include what changed, why, validation and remaining limits in PR descriptions.
  Verify the merge result before saying that a PR has merged.

## C++ and repository style

- Use C++20 and consistent `snake_case` names, including types, functions and
  members (`ark::plugin_manager`, `find_module()`, `module_name_`). Do not manually
  edit generated protobuf output; change the schema or generation source.
- Use lowercase `snake_case` for plugin CMake targets and shared-library stems,
  including official, server and sample plugins (`ark_mysql_plugin`,
  `sample1_plugin`). Keep application manifests, launch scripts and documentation
  aligned with the generated names; retain platform extensions and Debug `_d`.
- Follow `.clang-format` and the repository formatting scripts with clang-format
  18. Keep function opening braces on the declaration line, expand short function
  bodies, and use braces for control statements.
- Group includes as system, third-party, then project headers. Keep required
  platform-header dependencies in a valid order. Separate include blocks,
  namespaces and adjacent definitions with the prescribed blank lines.
- Run `CLANG_FORMAT=/path/to/clang-format bash scripts/format.sh --check`.
- CI policy tests: `node --test tests/ci/*.test.mjs`. Nimbus documentation:
  `npm ci --prefix docs/site` followed by `npm run check --prefix docs/site`.
- Keep official plugins in the ark repository and application business plugins
  in application repositories. The approved layout has one public header root,
  `include/ark/`, with implementation under `src/core/` and
  `src/plugins/<name>/`; retain independent plugin targets and optional builds.
  Verify migration status against accepted source before describing new paths
  as available. Do not expand the paused hot-update design without discussing
  its lifecycle and compatibility boundaries.

## Documentation and releases

- Keep English and Chinese documentation aligned. Keep the README concise and
  place detailed usage, architecture and validation material under `docs/`.
- Write release notes for users: summarize core capabilities and group relevant
  changes under Features, Improvements, Fixes and Upgrade notes. Omit empty
  sections and combine implementation details into their user-visible effect.
- Do not publish a commit-by-commit diary, file list, formatting list or test
  assertion inventory. Link detailed validation and performance reports instead.
  Retain compatibility changes, security requirements and known limitations.
- Only describe changes contained in the release tag. Verify the release commit,
  assets and checks before publishing; never move an existing release tag to
  make unreleased work appear included.
- Keep raw measurements and build logs outside the source repository. Published
  performance claims must identify the machine, workload, units and tested
  revision; local tests do not establish production PCU capacity.
