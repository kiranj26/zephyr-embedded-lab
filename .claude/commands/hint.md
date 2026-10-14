---
description: Get a progressive hint for the current lesson challenge (no spoilers)
argument-hint: <lesson number> [hint level 1-3]
---
I'm stuck on the challenge for lesson $1. Give me hint level ${2:-1} only:
- Level 1: a guiding question + which concept/doc/header to look at.
- Level 2: the specific API, macro, Kconfig symbol, or devicetree node I need, plus where it's used in the Zephyr tree
  (`/opt/toolchains/zephyr/samples/...` or `include/zephyr/...`).
- Level 3: pseudo-code or a skeleton with blanks. Still not the full solution.
First read my current code in `workspace/apps/$1*` so the hint fits what I've done. Never paste from `course/.../NN_solution_*`.
