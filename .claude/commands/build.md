---
description: Build an app in the container and explain any failure
argument-hint: <app prefix, e.g. 04 or course/04_demo_button>
allowed-tools: Bash(make build:*), Read, Grep, Glob
---
Run `make build APP=$ARGUMENTS`.
- If it succeeds, report the flash/RAM usage lines and the output path, then suggest the next step (`/flash-run $ARGUMENTS`).
- If it fails, delegate to the **build-doctor** subagent with the full error output. Then explain the root cause to me
  in plain words, point to the exact file/line, and propose the smallest fix. Ask before editing if this is my lesson code.
