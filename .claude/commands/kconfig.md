---
description: Investigate a Kconfig symbol (definition, dependencies, final value, why)
argument-hint: <SYMBOL without CONFIG_> [app prefix]
---
Use the **kconfig-detective** subagent to explain `CONFIG_$1`${2:+ for app $2}:
where it's defined, its type/default/help, `depends on` and `select` relationships, its final value in `.config`,
which file set it (board defconfig / prj.conf / board .conf / default), and how I'd change it properly.
