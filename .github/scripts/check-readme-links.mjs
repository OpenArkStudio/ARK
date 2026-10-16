import { existsSync, readFileSync } from 'node:fs';
import { dirname, isAbsolute, relative, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

function withoutCode(source) {
  let fence;
  return source.replace(/<!--[\s\S]*?-->/g, '').split('\n').filter(line => {
    const marker = line.match(/^ {0,3}(`{3,}|~{3,})/);
    if (marker) {
      if (!fence) fence = marker[1];
      else if (marker[1][0] === fence[0] && marker[1].length >= fence.length) fence = undefined;
      return false;
    }
    return !fence;
  }).join('\n').replace(/(`+)[^\n]*?\1/g, '');
}

function srcsetTargets(value) {
  const targets = [];
  let index = 0;
  while (index < value.length) {
    while (/[\s,]/.test(value[index] ?? '') && index < value.length) index++;
    const start = index;
    while (index < value.length && !/\s/.test(value[index])) index++;
    const candidate = value.slice(start, index);
    if (!candidate) break;
    targets.push(candidate.replace(/,+$/, ''));
    if (candidate.endsWith(',')) continue;
    // URL tokens may contain commas (notably data URLs); descriptors end at the next comma.
    while (index < value.length && value[index] !== ',') index++;
    if (index < value.length) index++;
  }
  return targets;
}

export function linkTargets(source) {
  const text = withoutCode(source);
  const targets = [];
  for (const match of text.matchAll(/\]\(\s*/g)) {
    let index = match.index + match[0].length;
    if (text[index] === '<') {
      const end = text.indexOf('>', index + 1);
      if (end !== -1) targets.push(text.slice(index + 1, end));
      continue;
    }
    let target = '';
    let depth = 0;
    for (; index < text.length; index++) {
      const character = text[index];
      if (character === '\\' && index + 1 < text.length) target += text[++index];
      else if (character === '(') { depth++; target += character; }
      else if (character === ')' && depth > 0) { depth--; target += character; }
      else if (character === ')' || /\s/.test(character)) break;
      else target += character;
    }
    targets.push(target);
  }
  for (const match of text.matchAll(/^ {0,3}\[[^\]\n]+\]:\s*(?:<([^>]+)>|(\S+))/gm)) targets.push(match[1] ?? match[2]);
  for (const match of text.matchAll(/\b(?:href|src)\s*=\s*(?:"([^"]*)"|'([^']*)')/gi)) targets.push(match[1] ?? match[2]);
  for (const match of text.matchAll(/\bsrcset\s*=\s*(?:"([^"]*)"|'([^']*)')/gi)) targets.push(...srcsetTargets(match[1] ?? match[2]));
  return targets;
}

export function checkReadmeLinks(root, files = ['README.md', 'README_CN.md']) {
  const failures = [];
  const repository = resolve(root);
  for (const file of files) {
    const source = resolve(repository, file);
    if (!existsSync(source)) { failures.push(`${file}: missing README`); continue; }
    for (const target of linkTargets(readFileSync(source, 'utf8'))) {
      // This check verifies repository-local file targets, not remote URLs or heading fragments.
      if (!target || target.startsWith('#') || target.startsWith('//') || /^[a-z][a-z0-9+.-]*:/i.test(target)) continue;
      try {
        const pathname = decodeURIComponent(target.split(/[?#]/, 1)[0]);
        const destination = pathname.startsWith('/') ? resolve(repository, `.${pathname}`) : resolve(dirname(source), pathname);
        const local = relative(repository, destination);
        if (local === '..' || local.startsWith('../') || isAbsolute(local) || !existsSync(destination)) {
          failures.push(`${file}: missing or outside-repository target ${JSON.stringify(target)}`);
        }
      } catch {
        failures.push(`${file}: invalid target ${JSON.stringify(target)}`);
      }
    }
  }
  return failures;
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  const failures = checkReadmeLinks(process.cwd());
  if (failures.length) {
    console.error(failures.join('\n'));
    process.exitCode = 1;
  } else console.log('Root README repository-local file targets passed; remote URLs and fragments were not fetched or validated.');
}
