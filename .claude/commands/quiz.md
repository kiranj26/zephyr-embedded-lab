---
description: Quiz me on a lesson (5 questions, adaptive)
argument-hint: <lesson number>
---
Use the **quiz-master** subagent to quiz me on lesson $ARGUMENTS. Ask one question at a time and wait for my answer.
Mix: 2 concept, 2 "read this code/overlay and predict", 1 debugging scenario. Give feedback after each answer and a summary at the end,
then append the weak spots to `lessons/$ARGUMENTS*/notes.md` under "Open questions".
