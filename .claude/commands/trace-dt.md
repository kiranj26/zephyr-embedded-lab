---
description: Trace a devicetree alias/node from overlay to generated C macros
argument-hint: <alias or node label> <app prefix>
---
Use the **devicetree-navigator** subagent. For `$1` in app `$2` (build it first if `workspace/build/*$2*` is missing):
1. Show where it's defined (board dts/dtsi vs my overlay) and the final merged node in `zephyr/zephyr.dts`.
2. Show the binding (`compatible` → `dts/bindings/...yaml`) and which properties are required.
3. Show the relevant `DT_N_...` macros in `devicetree_generated.h` and how `DT_ALIAS` / `GPIO_DT_SPEC_GET` reach them.
4. Summarize the chain in one diagram-like list.
