---
description: Start or resume a course lesson (e.g. /lesson 04)
argument-hint: <lesson number 01-12>
---
I'm starting/resuming lesson $ARGUMENTS of Introduction to Zephyr.

1. Read `lessons/$ARGUMENTS*/README.md` and the matching row in `docs/02-course-map.md`.
2. Read `PROGRESS.md` and `lessons/$ARGUMENTS*/notes.md` (if present) to see where I left off.
3. Give me a short briefing: the episode title, what I'll learn (3–5 bullets), hardware and wiring needed
   (cross-check pins with the `esp32s3-n16r8-hardware` skill), and which course folders it uses.
4. Ask me 2 quick warm-up questions about the previous lesson's key ideas.
5. Propose today's session plan (watch → run reference → build my own → challenge) with the exact `make` commands.
Do not show any solution code.
