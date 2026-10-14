---
description: Explain a file, concept, macro, or build artifact in course context
argument-hint: <path | concept | macro>
---
Explain `$ARGUMENTS` to me as a tutor:
- What it is and why Zephyr has it (one paragraph).
- How it works mechanically. If it's a macro, show what it expands to by checking the real header in the container
  (`docker exec zephyr-lab grep -rn ...` under /opt/toolchains/zephyr/include).
- Where it appears in the course (which lesson/app).
- One tiny experiment I can run to see it in action.
- 2 check-your-understanding questions.
