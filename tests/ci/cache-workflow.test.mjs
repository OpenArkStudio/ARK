import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';

const workflow = readFileSync(new URL('../../.github/workflows/ci.yml', import.meta.url), 'utf8').replaceAll('\r\n', '\n');
const profiles = [
  ['network', 'Configure network regressions', 'ci-build', ['check-network']],
  ['applications', 'Configure applications', 'ci-build', ['check-core']],
  ['database', 'Configure native database development tests', 'ci-database',
    ['check-database', 'check-database-benchmark']],
];

// Read this workflow's job/step indentation; this is not a general YAML parser.
function jobSteps(name) {
  const jobs = workflow.split(/(?=^  [a-z][a-z0-9_-]*:$)/m);
  const job = jobs.find(block => block.startsWith(`  ${name}:\n`));
  assert.ok(job, `missing job ${name}`);
  const [settings, ...steps] = job.split(/(?=^      - )/m);
  assert.doesNotMatch(settings, /^    continue-on-error:/m, `${name} must remain fatal`);
  assert.ok(steps.length > 0, `${name} must contain steps`);
  return steps;
}

function onlyStep(steps, predicate, description) {
  const found = steps.map((step, index) => ({ step, index })).filter(({ step }) => predicate(step));
  assert.equal(found.length, 1, `expected exactly one ${description}`);
  return found[0];
}

for (const [name, configureName, buildDirectory, checks] of profiles) {
  test(`${name} saves dependency archives immediately after successful configure`, () => {
    const steps = jobSteps(name);
    const configure = onlyStep(steps, step => step.startsWith(`      - name: ${configureName}\n`), 'configure step');
    const save = onlyStep(steps, step => step.includes('uses: actions/cache/save@v4\n'), 'archive save');
    const build = onlyStep(steps, step => step.includes(`run: cmake --build ${buildDirectory} --parallel `), 'source build');
    assert.equal(save.index, configure.index + 1, 'save only after configure has completed');
    assert.ok(save.index < build.index, 'save dependencies before compiling project sources');
    assert.match(save.step, /^        if: \$\{\{ success\(\) && steps\.vcpkg-cache\.outputs\.enabled == 'true' && steps\.vcpkg-restore\.outputs\.cache-hit != 'true' \}\}$/m);
    assert.match(save.step, /^        continue-on-error: true$/m);
    assert.match(save.step, /^          path: \$\{\{ steps\.vcpkg-cache\.outputs\.directory \}\}$/m);
    assert.match(save.step, /^          key: \$\{\{ steps\.vcpkg-cache\.outputs\.key \}\}$/m);
  });

  test(`${name} still requires configure, compilation and every regression command`, () => {
    const steps = jobSteps(name);
    const configure = onlyStep(steps, step => step.startsWith(`      - name: ${configureName}\n`), 'configure step');
    const build = onlyStep(steps, step => step.includes(`run: cmake --build ${buildDirectory} --parallel `), 'source build');
    const required = [configure, build, ...checks.map(target => onlyStep(steps,
      step => step.includes(`run: cmake --build ${buildDirectory} --target ${target}\n`), target))];
    assert.match(configure.step, /^          cmake -S \. -B /m);
    for (const { step } of required) {
      assert.doesNotMatch(step, /^        continue-on-error:/m, 'configure/build/test failure must fail the job');
      assert.doesNotMatch(step, /^        if:/m, 'configure/build/tests must retain their default success condition');
    }
    for (const check of required.slice(2)) {
      assert.ok(build.index < check.index, 'compile before running regressions');
    }
  });
}
