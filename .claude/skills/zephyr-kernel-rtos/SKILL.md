---
name: zephyr-kernel-rtos
description: Zephyr kernel API quick reference and pitfalls - threads, priorities, sleeping, mutexes, semaphores, message queues, k_timer, counter alarms, GPIO interrupts, and workqueues (k_work, k_work_delayable). Use for lessons 8-9 and any concurrency or timing code.
---
# Kernel cheat sheet (as used in Parts 8–9)

**Threads:** `K_THREAD_STACK_DEFINE(stack, SIZE)` + `static struct k_thread t;` +
`k_thread_create(&t, stack, K_THREAD_STACK_SIZEOF(stack), entry, a1, a2, a3, PRIO, 0, K_NO_WAIT)`.
Lower number = higher priority. Negative priority = cooperative (never preempted). `main` runs at priority 0 by default.
Or statically: `K_THREAD_DEFINE(name, SIZE, entry, a1, a2, a3, PRIO, 0, 0)`.

**Mutex:** `K_MUTEX_DEFINE(m)`, `k_mutex_lock(&m, K_FOREVER)` / `k_mutex_unlock(&m)`. Has priority inheritance. Never use it in an ISR.
**Semaphore:** `K_SEM_DEFINE(s, 0, 1)`, `k_sem_give` (ISR-safe), `k_sem_take(&s, K_FOREVER)`. Used for "event happened" signals (see wifi.c).
**Message queue:** `K_MSGQ_DEFINE(q, sizeof(item), N, 4)`, `k_msgq_put(&q, &item, K_NO_WAIT)` (ISR-safe with K_NO_WAIT), `k_msgq_get`.

**GPIO interrupt:** `gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE)`, `gpio_init_callback(&cb, isr, BIT(btn.pin))`,
`gpio_add_callback(btn.port, &cb)`. The callback runs in ISR context.
**k_timer:** `k_timer_init(&t, expiry_fn, NULL)`, `k_timer_start(&t, K_MSEC(d), K_MSEC(p))`. expiry_fn runs in ISR context.
**Counter (hardware timer0):** `counter_start`, `counter_set_channel_alarm` (one-shot, so re-arm it in the callback), `counter_us_to_ticks`.
**Workqueue:** `k_work_init_delayable(&w, handler)`; from an ISR call `k_work_reschedule(&w, K_MSEC(50))` to debounce, because every
bounce pushes the deadline out. The handler runs in the system workqueue thread, where it can sleep or log.

**ISR rules:** no blocking, no `K_FOREVER` waits, keep it short. Defer work with k_work, a semaphore, or a msgq.
**Stacks:** printk with formatting needs ~512 B+. Floats and LVGL need far more. Measure with `CONFIG_THREAD_ANALYZER=y`.
