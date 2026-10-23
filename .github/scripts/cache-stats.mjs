import { spawnSync } from 'node:child_process';
import { appendFileSync, readdirSync } from 'node:fs';

let archives = 'unavailable';
if (process.env.VCPKG_ARCHIVE_DIRECTORY) {
  try {
    archives = readdirSync(process.env.VCPKG_ARCHIVE_DIRECTORY, { recursive: true })
      .filter(file => file.endsWith('.zip')).length;
  } catch {
    // Cache diagnostics do not change the build or test result.
  }
}
const lines = [
  `Dependency archive cache exact hit: ${process.env.VCPKG_ARCHIVE_HIT || 'false/unavailable'}`,
  `Dependency archives after configure/build: ${archives}`,
  'vcpkg package restore/build counts are in the Configure step output.',
  `Compiler archive cache exact hit: ${process.env.COMPILER_ARCHIVE_HIT || 'false/unavailable'} (a prefix restore can still provide compiler hits)`,
];
if (process.env.ARK_COMPILER_LAUNCHER) {
  const result = spawnSync(process.env.ARK_COMPILER_LAUNCHER, ['--show-stats'], {
    encoding: 'utf8', timeout: 30_000,
  });
  lines.push(result.status === 0 ? result.stdout.trim() : 'sccache statistics unavailable');
} else {
  lines.push('Compiler cache disabled; compilation uses the compiler directly.');
}
const report = lines.join('\n');
console.log(report);
if (process.env.GITHUB_STEP_SUMMARY) appendFileSync(process.env.GITHUB_STEP_SUMMARY, `\n### Native build cache\n\n\`\`\`text\n${report}\n\`\`\`\n`);
