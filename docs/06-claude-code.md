# 06 · Using Claude Code in this repo

Run `claude` in the repo root. It reads `CLAUDE.md` (project memory) and `.claude/` automatically.

## Slash commands (`.claude/commands/`)
| Command | What it does |
|---|---|
| `/lesson 04` | Briefing, wiring check, warm-up questions, session plan |
| `/build 04` | Build; on failure the build-doctor agent explains the root cause |
| `/flash-run 04 [secs]` | Build → flash → capture serial → interpret |
| `/hint 04 [1-3]` | Progressive hints with no spoilers |
| `/review 04` | Code review + comparison with the course solution (after you've tried) |
| `/explain <thing>` | Tutor explanation grounded in the real Zephyr headers |
| `/trace-dt my-led 04` | Overlay → merged DTS → binding → generated macros |
| `/kconfig GPIO 04` | Where a symbol is defined, its dependencies, final value, and why |
| `/quiz 04` | Adaptive 5-question quiz; weak spots go into your notes |
| `/wire 10` | Wiring checklist + pin-safety check |
| `/new-app 13_idea` | Scaffold an app from the template |
| `/notes 04` · `/progress` | Write notes, update PROGRESS.md |

## Subagents (`.claude/agents/`)
`zephyr-tutor` (Socratic teaching) · `build-doctor` · `devicetree-navigator` · `kconfig-detective` · `firmware-reviewer` ·
`rtos-analyst` · `driver-mentor` · `hardware-safety` · `quiz-master`. Claude delegates to them automatically based on their
descriptions, or you can ask: "use the rtos-analyst on 08".

## Skills (`.claude/skills/*/SKILL.md`)
Knowledge Claude loads when relevant: `lab-workflow`, `esp32s3-n16r8-hardware`, `devicetree-overlays`, `kconfig-config`,
`zephyr-driver-module`, `zephyr-kernel-rtos`, `lvgl-display`, `zephyr-wifi-net`, `custom-board-hwmv2`, `openocd-gdb-debug`, `course-map`.

## Guardrails (`.claude/settings.json` + `.claude/hooks/`)
- **guard_course.py** (PreToolUse) blocks edits under `course/`.
- **pin_safety.py** (PostToolUse) runs `scripts/check_pins.py` after any `.overlay/.dts/.dtsi` edit and makes Claude fix forbidden pins.
- Permissions allow `make build/flash/capture/...` without prompting, and deny `make monitor`/`make run`
  (interactive, never return), `git push`, and reading `secrets.h`.
- Personal overrides go in `.claude/settings.local.json` (git-ignored).

## Tutor mode
`CLAUDE.md` tells Claude to teach first: hints before answers. Say **"show solution"** or **"just fix it"** when you want the answer.
