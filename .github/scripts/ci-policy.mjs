import { execFileSync } from 'node:child_process';
import { appendFileSync, existsSync, readFileSync, statSync } from 'node:fs';
import { posix, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const shaPattern = /^(?:[a-f0-9]{40}|[a-f0-9]{64})$/;
const cppPath = /\.(?:c|cc|cpp|cxx|c\+\+|h|hh|hpp|hxx|inl|ipp|tpp|proto)$/i;
const docPath = /\.(?:md|mdx|markdown|rst|adoc|txt|html|css|js|mjs|cjs|ts|tsx|jsx|astro|json|yaml|yml|svg|png|jpe?g|gif|webp|ico|pdf|woff2?|ttf|otf)$/i;
const dependencyName = /^(?:package(?:-lock)?\.json|npm-shrinkwrap\.json|pnpm-lock\.yaml|yarn\.lock|vcpkg(?:-configuration)?\.json|conanfile\.(?:py|txt)|conan\.lock|requirements(?:-[\w.-]+)?\.txt|.*\.lock|\.gitmodules)$/i;

export function isDocumentationPath(file) {
  if (typeof file !== 'string' || !file || /[\x00-\x1f\x7f\\]/.test(file) ||
      posix.isAbsolute(file) || posix.normalize(file) !== file || file.split('/').includes('..')) return false;
  const name = posix.basename(file);
  if (cppPath.test(file) || /^CMake(?:Lists\.txt|(?:User)?Presets\.json|Cache\.txt)$/i.test(name) || /\.cmake$/i.test(file) ||
      dependencyName.test(name) || file.split('/').includes('.github') || /^action\.ya?ml$/i.test(name) ||
      /^\.clang-(?:format|tidy)$/.test(name)) return false;
  if (file === 'README.md' || file === 'README_CN.md') return true;
  return file.startsWith('docs/') && (docPath.test(file) || name === 'LICENSE' || name === '.gitignore');
}

export function decodePaths(buffer) {
  const text = new TextDecoder('utf-8', { fatal: true }).decode(buffer);
  if (text && !text.endsWith('\0')) throw new Error('truncated Git path list');
  const files = text ? text.slice(0, -1).split('\0') : [];
  if (files.some(file => !file) || new Set(files).size !== files.length) throw new Error('invalid Git path list');
  return files;
}

export function changedPaths({ root, base, head, pullRequest = true }, runGit) {
  if (typeof base !== 'string' || typeof head !== 'string' || !shaPattern.test(base) || !shaPattern.test(head) ||
      /^0+$/.test(base) || /^0+$/.test(head)) {
    throw new Error('missing or invalid commit identity');
  }
  const git = runGit ?? (args => execFileSync('git', args, {
    cwd: root, maxBuffer: 32 * 1024 * 1024, timeout: 60_000, stdio: ['ignore', 'pipe', 'pipe'],
  }));
  if (git(['rev-parse', '--is-shallow-repository']).toString().trim() !== 'false') {
    throw new Error('incomplete Git history');
  }
  git(['cat-file', '-e', `${base}^{commit}`]);
  git(['cat-file', '-e', `${head}^{commit}`]);
  let start = base;
  if (pullRequest) {
    const ancestors = git(['merge-base', '--all', base, head]).toString().trim().split(/\s+/);
    if (ancestors.length !== 1 || !shaPattern.test(ancestors[0])) throw new Error('ambiguous merge base');
    start = ancestors[0];
  }
  // Renames become deletion + addition, so a move into docs cannot hide old code.
  return decodePaths(git(['diff', '--no-ext-diff', '--no-textconv', '--no-renames', '--name-only', '-z', start, head, '--']));
}

export function inspectSite(root) {
  const site = resolve(root, 'docs/site');
  const present = existsSync(site);
  let available = false;
  try {
    const manifest = JSON.parse(readFileSync(resolve(site, 'package.json'), 'utf8'));
    available = statSync(resolve(site, 'package-lock.json')).isFile() &&
      typeof manifest.scripts?.check === 'string' && manifest.scripts.check.trim().length > 0;
  } catch {
    // A missing legacy site forces full C++; an introduced/broken site must fail its own job.
  }
  return { present, available };
}

export function selectScope({ eventName, files, site, enumerationFailed = false }) {
  const known = !enumerationFailed && Array.isArray(files);
  const paths = known ? files : [];
  const nonempty = known && paths.length > 0;
  const docsOnly = eventName === 'pull_request' && nonempty && site.available && paths.every(isDocumentationPath);
  const readme = paths.some(file => file === 'README.md' || file === 'README_CN.md');
  const docsTouched = readme || paths.some(file => file.startsWith('docs/'));
  const ciTouched = paths.some(file => file.startsWith('.github/') || file.startsWith('tests/ci/'));
  const siteReferenced = site.present || paths.some(file => file.startsWith('docs/site/'));
  const docs = siteReferenced && (eventName !== 'pull_request' || !known || docsTouched || ciTouched);
  return {
    full_cpp: !docsOnly,
    docs_check: docs,
    readme_check: readme,
    site_available: site.available,
    files_known: known,
    reason: docsOnly ? 'documentation-only' : eventName !== 'pull_request' ? 'full-event' :
      !known ? 'enumeration-unavailable' : !nonempty ? 'empty-diff' :
        !site.available ? 'site-unavailable' : 'code-build-or-unknown-path',
  };
}

export function validateGate(needs) {
  if (!needs || needs.scope?.result !== 'success') throw new Error('scope job did not succeed');
  const flags = {};
  for (const key of ['full_cpp', 'docs_check', 'readme_check', 'site_available', 'files_known']) {
    const value = needs.scope.outputs?.[key];
    if (value !== 'true' && value !== 'false') throw new Error(`invalid scope output: ${key}`);
    flags[key] = value === 'true';
  }
  if (!flags.full_cpp && (!flags.docs_check || !flags.site_available || !flags.files_known)) {
    throw new Error('documentation-only scope is incomplete');
  }
  const required = {
    network: flags.full_cpp, applications: flags.full_cpp, database: flags.full_cpp,
    documentation: flags.docs_check, readme: flags.readme_check,
  };
  for (const [job, mustRun] of Object.entries(required)) {
    const expected = mustRun ? 'success' : 'skipped';
    if (needs[job]?.result !== expected) throw new Error(`${job}: expected ${expected}, got ${needs[job]?.result ?? 'missing'}`);
  }
}

function classify() {
  const root = process.cwd();
  const eventName = process.env.GITHUB_EVENT_NAME;
  let files;
  let enumerationFailed = false;
  try {
    const event = JSON.parse(readFileSync(process.env.GITHUB_EVENT_PATH, 'utf8'));
    if (eventName === 'pull_request') {
      files = changedPaths({ root, base: event.pull_request?.base?.sha, head: event.pull_request?.head?.sha });
    } else if (eventName === 'push') {
      files = changedPaths({ root, base: event.before, head: event.after, pullRequest: false });
    } else {
      enumerationFailed = true;
    }
  } catch {
    enumerationFailed = true;
  }
  const decision = selectScope({ eventName, files, site: inspectSite(root), enumerationFailed });
  const output = Object.entries(decision).map(([key, value]) => `${key}=${value}\n`).join('');
  if (process.env.GITHUB_OUTPUT) appendFileSync(process.env.GITHUB_OUTPUT, output);
  const summary = `CI scope: ${decision.reason}\n\n${output}\n` +
    (!decision.site_available ? (decision.docs_check ? 'The required Nimbus site is missing or invalid; its job must fail.\n' :
      'Nimbus is not introduced in this checkout; site validation is explicitly not required and C++ remains full.\n') : '');
  if (process.env.GITHUB_STEP_SUMMARY) appendFileSync(process.env.GITHUB_STEP_SUMMARY, summary);
  console.log(JSON.stringify({ ...decision, file_count: files?.length ?? null }, null, 2));
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  try {
    if (process.argv[2] === 'classify') classify();
    else if (process.argv[2] === 'gate') {
      validateGate(JSON.parse(process.env.CI_NEEDS_JSON));
      console.log('Every required job succeeded; only explicitly unnecessary jobs were skipped.');
    } else if (process.argv[2] === 'require-site') {
      if (!inspectSite(process.cwd()).available) throw new Error('Required Nimbus package, lockfile or check script is unavailable');
    } else throw new Error('Expected classify, gate or require-site');
  } catch (error) {
    console.error(error.message);
    process.exitCode = 1;
  }
}
