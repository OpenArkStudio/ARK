// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { resolve, relative, sep } from 'node:path';
import { createHash } from 'node:crypto';

const root = fileURLToPath(new URL('../', import.meta.url));
const directory = process.argv[2] ? resolve(process.argv[2]) : resolve(root, 'benchmarks/results/macmini-m4pro-20261015/receive-buffers');
const meta = JSON.parse(readFileSync(resolve(directory, 'metadata.json'), 'utf8'));
const sha = path => createHash('sha256').update(readFileSync(path)).digest('hex');
for (const [path, hash] of Object.entries(meta.optimized.source_sha256)) {
  if (sha(resolve(root, path)) !== hash) throw new Error(`Measured source differs: ${path}`);
}
if (sha(resolve(root, 'benchmarks/network_buffers.cpp')) !== meta.optimized.benchmark_sha256 ||
    meta.optimized.benchmark_sha256 !== meta.baseline.benchmark_sha256) throw new Error('Benchmark source mismatch');
const rows = Object.fromEntries(['before', 'after'].map(side => [side,
  readFileSync(resolve(directory, `${side}.jsonl`), 'utf8').trim().split('\n').map(JSON.parse)]));
if (Object.values(rows).some(values => values.length !== 24 || values.some(row => !row.ok))) throw new Error('Expected 48 successful paired measurements');
const key = row => [row.mode, row.payload_bytes, row.chunk_bytes, row.batch_messages].join('/');
const ids = [...new Set(rows.before.map(key))];
const median = values => [...values].sort((a, b) => a - b)[Math.floor(values.length / 2)];
const groups = ids.map(id => {
  const result = { id };
  for (const side of ['before', 'after']) {
    const samples = rows[side].filter(row => key(row) === id);
    if (samples.length !== meta.repetitions) throw new Error(`Missing repetitions: ${id}`);
    const rates = samples.map(row => row.operations_per_second);
    result[side] = { samples, rate: median(rates), min: Math.min(...rates), max: Math.max(...rates),
      cpu: median(samples.map(row => row.cpu_percent)), allocations: median(samples.map(row => row.cpp_object_allocations / row.operations)) };
  }
  result.change = (result.after.rate / result.before.rate - 1) * 100;
  return result;
});
const capacityRows = readFileSync(resolve(directory, 'configurations.jsonl'), 'utf8').trim().split('\n').map(JSON.parse);
const capacityWorkloads = ['framing/1024/64/1', 'framing/4096/65536/16'];
const capacities = meta.configured_capacities_bytes;
if (capacityRows.length !== capacityWorkloads.length * capacities.length * meta.repetitions ||
    capacityRows.some(row => !row.ok || !capacityWorkloads.includes(key(row)) || !capacities.includes(row.initial_buffer_override_bytes))) {
  throw new Error('Unexpected capacity measurements');
}
const capacityGroups = capacityWorkloads.map(id => ({ id, samples: capacities.map(capacity => {
  const samples = capacityRows.filter(row => key(row) === id && row.initial_buffer_override_bytes === capacity);
  if (samples.length !== meta.repetitions) throw new Error(`Missing capacity repetitions: ${id}/${capacity}`);
  const rates = samples.map(row => row.operations_per_second);
  if (rates.some(rate => !Number.isFinite(rate) || rate <= 0)) throw new Error('Invalid capacity throughput');
  return { capacity, rate: median(rates), min: Math.min(...rates), max: Math.max(...rates), cpu: median(samples.map(row => row.cpu_percent)) };
}) }));
const measurementCount = rows.before.length + rows.after.length + capacityRows.length;
if (measurementCount !== meta.measurements) throw new Error('Measurement metadata count differs');
const format = (value, digits = 0) => value.toLocaleString('en-US', { maximumFractionDigits: digits, minimumFractionDigits: digits });
const esc = value => String(value).replaceAll('&', '&amp;').replaceAll('<', '&lt;').replaceAll('>', '&gt;');
const out = resolve(root, 'docs/performance');
mkdirSync(resolve(out, 'assets'), { recursive: true });
const memoryCount = groups[0].before.samples[0].operations;
const memoryBefore = median(groups[0].before.samples.map(row => row.cpp_array_requested_bytes)) / 2 ** 20;
const memoryAfter = median(groups[0].after.samples.map(row => row.cpp_array_requested_bytes)) / 2 ** 20;
const memoryReduction = (1 - memoryAfter / memoryBefore) * 100;
const dataLink = `../../${relative(root, directory).split(sep).join('/')}`;

for (const zh of [false, true]) {
  const suffix = zh ? '_CN' : '';
  const labels = zh ? ['原实现', '优化实现'] : ['Baseline', 'Optimized'];
  const selected = [groups[0], ...['framing/1024/64/1', 'framing/4096/1/1', 'framing/4096/65536/16'].map(id => groups.find(group => group.id === id))];
  const headings = zh ? ['每会话初始收包缓冲', '分次输入：1 KiB 消息', '逐字节输入：4 KiB 消息', '批量输入：每批 16 条'] :
    ['Initial receive buffer per session', '1 KiB messages in 64 B blocks', '4 KiB messages in 1 B blocks', '16 messages per batch'];
  const subtitles = zh ? ['10000 个会话对象；缓冲按需增长', '输入块 64 B；每批 1 条', '输入块 1 B；每批 1 条', '消息体 4 KiB；输入块 64 KiB'] :
    ['10,000 session objects; buffers grow as needed', '64 B input blocks; one message per batch', '1 B input blocks; one message per batch', '4 KiB bodies; 64 KiB input blocks'];
  const parts = [`<svg xmlns="http://www.w3.org/2000/svg" width="960" height="640" viewBox="0 0 960 640" role="img" aria-labelledby="title description">`,
    `<title id="title">${zh ? 'ark 收包缓冲与拆包基准' : 'ark receive-buffer and framing benchmarks'}</title>`,
    `<desc id="description">${zh ? '三次独立运行的中位数；横线为最小值与最大值。内存图统计初始数组申请容量。同时包含完整批量输入组。' : 'Three-run medians; whiskers show minimum and maximum. The memory panel counts initial array capacity requests. The batch workload is also included.'}</desc>`,
    `<style>text{font-family:system-ui,-apple-system,"Segoe UI",sans-serif;fill:#0f172a} .muted{fill:#475569;font-size:12px}</style>`,
    '<rect width="960" height="640" fill="#f8fafc"/>',
    `<text x="20" y="30" font-size="22" font-weight="650">${zh ? 'ark 收包缓冲与拆包基准' : 'ark receive-buffer and framing benchmarks'}</text>`,
    `<text x="940" y="53" text-anchor="end" class="muted">${zh ? '3 次测量：中位数与最小／最大范围' : '3 runs: median and min/max range'}</text>`];
  selected.forEach((group, panel) => {
    const memory = panel === 0;
    const stats = ['before', 'after'].map(side => memory ? { rate: group[side].samples[0].cpp_array_requested_bytes / group[side].samples[0].operations / 1024, min: 0, max: 0 } : group[side]);
    const rawMax = Math.max(...stats.map(stat => memory ? stat.rate : stat.max)) * 1.1;
    const magnitude = 10 ** Math.floor(Math.log10(rawMax));
    const maximum = [1, 1.5, 2, 2.5, 3, 4, 5, 6, 8, 10].find(value => value * magnitude >= rawMax) * magnitude;
    const unit = memory ? 'KiB' : maximum >= 1000000 ? 'M msg/s' : 'k msg/s';
    const divisor = memory ? 1 : maximum >= 1000000 ? 1000000 : 1000;
    const x = 20 + (panel % 2) * 480, y = 75 + Math.floor(panel / 2) * 275;
    parts.push(`<g transform="translate(${x} ${y})"><rect width="440" height="250" rx="12" fill="white" stroke="#e2e8f0"/>`,
      `<text x="18" y="28" font-size="16" font-weight="600">${esc(headings[panel])}</text>`,
      `<text x="18" y="49" class="muted">${esc(subtitles[panel])}</text>`);
    stats.forEach((stat, index) => {
      const barY = 84 + index * 62;
      const scale = value => 90 + value / maximum * 320;
      const color = index ? '#2563eb' : '#94a3b8';
      parts.push(`<text x="18" y="${barY + 18}" font-size="12">${labels[index]}</text>`,
        `<text x="410" y="${barY - 9}" text-anchor="end" font-size="13" font-weight="600">${format(stat.rate / divisor, memory ? 0 : 2)} ${unit}</text>`,
        `<rect x="90" y="${barY}" width="${Math.max(1, stat.rate / maximum * 320).toFixed(2)}" height="26" rx="3" fill="${color}"/>`);
      if (!memory) parts.push(`<path d="M${scale(stat.min)} ${barY + 13}H${scale(stat.max)} M${scale(stat.min)} ${barY + 7}V${barY + 19} M${scale(stat.max)} ${barY + 7}V${barY + 19}" fill="none" stroke="#0f172a" stroke-width="1.5"/>`);
    });
    parts.push('<path d="M90 201H410" stroke="#cbd5e1"/>');
    for (const fraction of [0, 0.5, 1]) parts.push(`<text x="${90 + fraction * 320}" y="222" text-anchor="middle" class="muted">${format(maximum * fraction / divisor, memory ? 0 : 1)}</text>`);
    parts.push(`<text x="410" y="240" text-anchor="end" class="muted">${unit}</text></g>`);
  });
  parts.push('</svg>');
  writeFileSync(resolve(out, `assets/receive-buffers${suffix}.svg`), parts.join('\n') + '\n');
  const capacityParts = [
    '<svg xmlns="http://www.w3.org/2000/svg" width="960" height="340" viewBox="0 0 960 340" role="img" aria-labelledby="title description">',
    `<title id="title">${zh ? '初始缓冲容量与拆包吞吐量' : 'Initial capacity and framing throughput'}</title>`,
    `<desc id="description">${zh ? '4、64、512 KiB 初始缓冲容量的三次测量中位数，横线为最小值和最大值。' : 'Three-run medians for 4, 64 and 512 KiB initial buffers. Whiskers show minimum and maximum.'}</desc>`,
    '<style>text{font-family:system-ui,-apple-system,"Segoe UI",sans-serif;fill:#0f172a}.muted{fill:#475569;font-size:12px}</style>',
    '<rect width="960" height="340" fill="#f8fafc"/>',
    `<text x="20" y="30" font-size="22" font-weight="650">${zh ? '初始容量选择 · 相同拆包实现' : 'Initial capacity · same framing implementation'}</text>`
  ];
  capacityGroups.forEach((group, index) => {
    const maximum = Math.ceil(Math.max(...group.samples.map(sample => sample.max)) / 1000000) * 1000000;
    const scale = value => 90 + value / maximum * 320;
    const heading = index === 0 ? (zh ? '1 KiB 消息 · 64 B 输入块 · 每批 1 条' : '1 KiB body · 64 B blocks · batch 1') :
      (zh ? '4 KiB 消息 · 64 KiB 输入块 · 每批 16 条' : '4 KiB body · 64 KiB blocks · batch 16');
    capacityParts.push(`<g transform="translate(${20 + index * 480} 55)"><rect width="440" height="265" rx="12" fill="white" stroke="#e2e8f0"/>`,
      `<text x="18" y="27" font-size="15" font-weight="600">${heading}</text>`);
    group.samples.forEach((sample, position) => {
      const y = 65 + position * 57;
      capacityParts.push(`<text x="18" y="${y + 17}" font-size="12">${sample.capacity / 1024} KiB</text>`,
        `<text x="410" y="${y - 7}" text-anchor="end" font-size="13" font-weight="600">${format(sample.rate / 1000000, 2)} M msg/s</text>`,
        `<rect x="90" y="${y}" width="${(sample.rate / maximum * 320).toFixed(2)}" height="25" rx="3" fill="${['#2563eb', '#0d9488', '#7c3aed'][position]}"/>`,
        `<path d="M${scale(sample.min)} ${y + 12.5}H${scale(sample.max)} M${scale(sample.min)} ${y + 6}V${y + 19} M${scale(sample.max)} ${y + 6}V${y + 19}" stroke="#0f172a" stroke-width="1.5" fill="none"/>`);
    });
    capacityParts.push('<path d="M90 228H410" stroke="#cbd5e1"/>');
    for (const fraction of [0, 0.5, 1]) capacityParts.push(`<text x="${90 + fraction * 320}" y="249" text-anchor="middle" class="muted">${format(maximum * fraction / 1000000, 1)}</text>`);
    capacityParts.push('</g>');
  });
  capacityParts.push('</svg>');
  writeFileSync(resolve(out, `assets/receive-buffer-capacities${suffix}.svg`), capacityParts.join('\n') + '\n');
  const capacityLines = [zh ? '## 初始容量选择' : '## Initial capacity selection', '',
    `![${zh ? '三种初始容量的拆包吞吐量' : 'Framing throughput with three initial capacities'}](assets/receive-buffer-capacities${suffix}.svg)`, '',
    zh ? '| 消息体 / B | 输入块 / B | 每批消息 | 初始容量 / KiB | 吞吐量 / msg/s | CPU / % |' :
      '| Body / B | Input block / B | Batch | Initial capacity / KiB | Throughput / msg/s | CPU / % |',
    '| ---: | ---: | ---: | ---: | ---: | ---: |'];
  for (const group of capacityGroups) {
    const [, body, chunk, batch] = group.id.split('/');
    for (const sample of group.samples) capacityLines.push(`| ${body} | ${chunk} | ${batch} | ${sample.capacity / 1024} | ${format(sample.rate)} | ${format(sample.cpu, 1)} |`);
  }
  capacityLines.push('', zh ? '默认初始容量为 4 KiB，缓冲按需增长。大批量输入可在端点启动前通过 `set_initial_receive_buffer_size(bytes)` 配置较大容量；零值或运行中修改返回 `false`。此容量是应用层拆包缓冲，与 socket 收发缓冲分开设置。' :
    'The default initial capacity is 4 KiB, growing as needed. For large batches, call `set_initial_receive_buffer_size(bytes)` before starting the endpoint. A zero value or changes while running return `false`. This application framing buffer is configured separately from socket send/receive buffers.', '');
  const lines = [zh ? '# 收包缓冲与拆包性能' : '# Receive-buffer and framing performance', '',
    zh ? `在 ${meta.date} 的本机 Release 构建中完成 ${measurementCount} 次测量。图表与表格保留全部配置，数据来自独立 C++ 程序。` :
      `${measurementCount} measurements were run locally on ${meta.date} using Release builds and the standalone C++ program. All configurations are retained.`, '',
    `![${zh ? '初始缓冲与拆包吞吐量，中位数和测量范围' : 'Initial buffers and decoding throughput, medians and ranges'}](assets/receive-buffers${suffix}.svg)`, '',
    zh ? '## 测试环境' : '## Environment', '',
    '| ' + (zh ? '项目 | 配置' : 'Item | Configuration') + ' |', '| --- | --- |',
    `| ${zh ? '机器' : 'Machine'} | Mac mini ${meta.machine.model}, ${meta.machine.cpu}, ${meta.machine.logical_cores} ${zh ? '核' : 'cores'}, ${meta.machine.memory_bytes / 2 ** 30} GiB |`,
    `| OS | macOS ${meta.machine.os} (${meta.machine.os_build}), ${meta.machine.architecture} |`,
    `| ${zh ? '编译器' : 'Compiler'} | ${meta.compiler} |`, `| ${zh ? '构建' : 'Build'} | ${meta.build}, ${meta.language}, ${meta.dependency} |`,
    `| ${zh ? '重复与预热' : 'Repetitions and warmup'} | ${meta.repetitions} ${zh ? '次；拆包每次预热 10000 批' : 'runs; 10,000 warmup batches per framing run'} |`, '',
    zh ? '## 结果' : '## Results', '',
    zh ? `${format(memoryCount)} 个会话的初始收包缓冲申请量：**${format(memoryBefore)} MiB → ${format(memoryAfter, 1)} MiB（减少 ${format(memoryReduction, 2)}%）**。这是应用层缓冲数组的容量；socket 缓冲、队列和其他对象另计。` :
      `Initial receive-buffer requests for ${format(memoryCount)} session objects: **${format(memoryBefore)} MiB → ${format(memoryAfter, 1)} MiB (${format(memoryReduction, 2)}% less)**. This counts application buffer arrays; socket buffers, queues and other objects are separate.`, '',
    zh ? '| 消息体 / B | 输入块 / B | 每批消息 | 原实现 / msg/s | 优化实现 / msg/s | 变化 | 原 CPU / % | 优化 CPU / % |' :
      '| Body / B | Input block / B | Batch | Baseline / msg/s | Optimized / msg/s | Change | Baseline CPU / % | Optimized CPU / % |',
    '| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |'];
  for (const group of groups.slice(1)) {
    const row = group.before.samples[0];
    lines.push(`| ${row.payload_bytes} | ${row.chunk_bytes} | ${row.batch_messages} | ${format(group.before.rate)} | ${format(group.after.rate)} | ${group.change >= 0 ? '+' : ''}${format(group.change, 1)}% | ${format(group.before.cpu, 1)} | ${format(group.after.cpu, 1)} |`);
  }
  const partial = groups.find(group => group.id === 'framing/1024/64/1');
  const bytewise = groups.find(group => group.id === 'framing/4096/1/1');
  const batch = groups.find(group => group.id === 'framing/4096/65536/16');
  lines.push('', zh ? '- 1 KiB / 64 B 输入块：每消息普通对象分配 **' + partial.before.allocations + ' → ' + partial.after.allocations + '**。' :
      '- 1 KiB / 64 B blocks: ordinary object allocations per message **' + partial.before.allocations + ' → ' + partial.after.allocations + '**.',
    zh ? '- 4 KiB / 逐字节输入：每消息普通对象分配 **' + bytewise.before.allocations + ' → ' + bytewise.after.allocations + '**。' :
      '- 4 KiB / bytewise input: ordinary object allocations per message **' + bytewise.before.allocations + ' → ' + bytewise.after.allocations + '**.',
    zh ? `- 4 KiB / 64 KiB 输入块 / 每批 16 条：吞吐量变化 **${format(batch.change, 1)}%**，作为内存与吞吐量的取舍保留。` :
      `- 4 KiB / 64 KiB blocks / batch 16: throughput changed **${format(batch.change, 1)}%**; the memory/throughput tradeoff is retained.`, '',
    zh ? 'CPU 100% 代表占满一个逻辑核心。程序单线程运行，计时包含消息内容校验。连接容量、网络吞吐量和 RTT 需另做端到端验收。' :
      'CPU 100% represents one logical core. This single-threaded program includes message validation in its timing. Connection capacity, network throughput and RTT require end-to-end acceptance.', '',
    ...capacityLines,
    zh ? '## 复现与原始记录' : '## Reproduction and raw records', '',
    `- [${zh ? '构建与运行参数' : 'Build and run options'}](../../benchmarks/README${suffix}.md)`,
    `- [${zh ? '原实现数据' : 'Baseline data'}](${dataLink}/before.jsonl) · [${zh ? '优化实现数据' : 'Optimized data'}](${dataLink}/after.jsonl)`,
    `- [${zh ? '初始容量对比数据' : 'Capacity comparison data'}](${dataLink}/configurations.jsonl)`,
    `- [${zh ? '机器、源码与二进制校验值' : 'Machine, source and binary hashes'}](${dataLink}/metadata.json)`,
    `- [${zh ? '原实现提交' : 'Baseline commit'}](https://github.com/OpenArkStudio/ark/commit/${meta.baseline_commit})`, '',
    zh ? '对照版本使用同一份基准源码。每轮依次运行原实现、默认容量的优化实现和各容量配置，共三轮；每组独立启动进程，测量期间没有并行编译。' :
      'Both implementations use the same benchmark source. Each of three rounds runs the baseline, optimized default capacity and capacity cases in that order. Each case starts a fresh process, with no concurrent compilation.', '');
  writeFileSync(resolve(out, `receive-buffers${suffix}.md`), lines.join('\n'));
}
console.log(`Generated bilingual reports and SVGs from ${groups.length} paired cases, ${capacityGroups.length * capacities.length} capacity cases and ${measurementCount} measurements.`);
