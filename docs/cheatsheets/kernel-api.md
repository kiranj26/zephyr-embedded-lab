# Cheatsheet · Kernel API
`k_msleep(ms)` · `k_sleep(K_MSEC(x))` · `k_uptime_get()` · `K_FOREVER` / `K_NO_WAIT`
Threads: `K_THREAD_STACK_DEFINE`, `k_thread_create`, `K_THREAD_DEFINE`, `k_thread_name_set`. Priority: lower = more urgent; <0 = cooperative.
Sync: `K_MUTEX_DEFINE` + lock/unlock · `K_SEM_DEFINE` + give/take · `atomic_t` + `atomic_inc/get`.
Data: `K_MSGQ_DEFINE(q, size, n, align)` + `k_msgq_put/get/num_used_get`.
Time: `k_timer_init/start/stop` (expiry runs in ISR context) · `k_work_init_delayable` + `k_work_reschedule`.
GPIO IRQ: `gpio_pin_interrupt_configure_dt`, `gpio_init_callback`, `gpio_add_callback`.
Logging: `LOG_MODULE_REGISTER(name, LOG_LEVEL_INF)`, `LOG_INF/WRN/ERR/DBG`.
