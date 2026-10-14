# TODO 2: Blink "SOS" in Morse

A learning guide covering what to build, the ideas behind it, and the decisions to make.
There's no solution code here. You write that in `workspace/apps/01_blink/src/main.c`.

---

## 1. What you're building

Right now the LED does a fixed **on 1 s, off 1 s, repeat**. Make it signal the Morse distress code instead:

```
S       O       S
· · ·   — — —   · · ·      (pause, then repeat)
```

The output is still **one LED going on and off**. The only new thing is **timing**: when to switch it, and for how long.
The real lesson is moving from a hard-coded loop to a *pattern* the code follows.

---

## 2. The Morse timing rules

Morse is built from one **base unit** of time. Everything else is a multiple of it:

| Element | LED | Length |
|---|---|---|
| dot `·` | ON | 1 unit |
| dash `—` | ON | 3 units |
| gap between elements *inside* a letter | OFF | 1 unit |
| gap between **letters** | OFF | 3 units |
| gap between **words** (here: between repeats of SOS) | OFF | 7 units |

**Note:** every ON is followed by an OFF. The only question is *how long* that OFF is,
and that depends on what comes next.

### Exercise A (on paper, before any code)
Pick a unit, for example 200 ms, and draw one full SOS cycle as a timeline of ON/OFF blocks:
```
ON  OFF ON  OFF ON  OFF ...
1   1   1   1   1   3   ...
```
Then answer:
1. How many **units** long is one full SOS, including the gap before it repeats?
2. With your unit, how many **milliseconds** is that?
3. Count the separate ON blocks and OFF blocks.

You'll use these numbers in step 6 to check your firmware.

---

## 3. What you already know (and can reuse)

| You need to… | You already have… |
|---|---|
| turn the LED on or off | `gpio_pin_set_dt(&led, 1 or 0)` |
| wait for a time | `k_msleep(ms)` |
| check how long things really took | the tick-based timestamps from TODO 1 |
| know that sleep lasts slightly longer than requested | your TODO 1 result: **+1 tick (0.1 ms) per sleep** |

Nothing new from Zephyr is needed. This one is about **program design**.

---

## 4. Decisions to make (write your choice and why in notes.md)

### Decision 1: How do you describe the pattern?

**Option (a): helper functions**
Write small functions like "do a dot" and "do a dash", then call them in order for S, O, S.
- Easy to read today.
- Changing the message means writing new calls.

**Option (b): data plus one player loop**
Store the pattern as **data**, for example an array, and have a single loop that walks through it and plays each entry.
- A new message means changing data, not code.
- Takes more thought about *what one entry represents*.

Questions to settle for (b):
- Is one entry one *element* (dot or dash), one *ON or OFF period*, or one *character*?
- How do you know where the pattern ends? Store a length, use `ARRAY_SIZE()`, or put a marker at the end?
- Where do the letter and word gaps live: in the data, or in the player's logic?

You can start with (a) and refactor to (b) later. Doing both is the best way to learn the difference.

### Decision 2: Name your timing constants
Dot, dash, the three gaps and the base unit are **magic numbers**. Give them names, and define everything **from the unit**,
so changing one number speeds up or slows down the whole pattern.
*Question:* `#define`, `static const`, or an `enum`? What's the difference for the compiler and for the debugger?

### Decision 3: What happens to the old 1 Hz blink?
- Replace it?
- Keep both, with a `#define` switch?
- Keep both, with a **Kconfig option** in `prj.conf`? This is a preview of lesson 03, and needs a `Kconfig` file in the app. You can come back to this one after lesson 03.

### Decision 4: Error handling in helpers
`gpio_pin_set_dt()` returns an error code. If it fails *inside* a helper function, how does `main()` find out?
Return it? Ignore it? Log it and continue? Pick one deliberately. All three are seen in real firmware.

### Decision 5: What should `printk` say?
A line per ON/OFF floods the console (about 18 lines per SOS). Options: one line per letter, one per full SOS,
or none during the pattern and a summary after it.
*Hint:* TODO 1 showed printk is nearly free while the UART FIFO has room. How many characters fit before it
starts blocking, and could a chatty pattern hit that?

---

## 5. Suggested build order (small, checkable steps)

1. **Constants:** add the unit and the derived timings. Build. Nothing changes yet.
2. **One dot, in a loop:** replace the 1 Hz blink with ON for one unit, then OFF for one unit, forever.
   Flash it and *look*: does it blink fast and evenly?
3. **The letter S:** three dots, then the letter gap. Can you see the pause?
4. **The letter O:** three dashes. Compare the ON length with S by eye. Do dashes look about 3 times as long?
5. **The full SOS plus word gap,** repeating.
6. **Measure it:** print one timestamp per full SOS cycle (reuse the TODO 1 technique).
   Compare with your Exercise A answer.
7. **(Optional) Refactor** to the other design option from Decision 1.

Build, flash and check after **each** step:
```sh
make build APP=01 && make flash APP=01 && make capture SECS=10
```

---

## 6. How to know you're done

- [x] Looking at it, you can clearly read `· · ·  — — —  · · ·` with a longer pause before it repeats
- [x] Every timing is defined from one base unit. Changing it changes the speed of everything
- [x] The measured cycle time matches your paper calculation from Exercise A
- [x] Your measurement is slightly **longer** than the paper value, and you can explain by how much
      (*hint:* count how many `k_msleep` calls are in one cycle and remember the +1 tick each)
- [x] `gpio_pin_set_dt` errors are handled the way you decided
- [x] No compiler warnings
- [x] Header TODO ticked, and your design choices written in `notes.md`

---

## 7. Stretch goals (pick any)

- **Any message:** make the player blink any text, for example `"HELLO"`. You'll need a lookup table from letters to Morse.
  Where should it live, and how big is it?
- **No drift:** make each cycle start at an exact time, so the error from TODO 1 can't accumulate.
  Look in `kernel.h` for a way to sleep **until an absolute time**, instead of *for* a duration.
- **Second LED preview:** once TODO 3 is done, blink the dots on one LED and the dashes on the other.

---

## 8. Common traps

- **Forgetting the OFF after the last element of a letter.** The letter gap *is* that OFF. Don't add an extra 1-unit gap as well.
- **A unit that's too small.** Under about 50 ms, your eye can't tell dots from dashes. Start with 150–250 ms.
- **Integer division:** if you derive timings with `/`, check you aren't silently rounding.
- **Port busy when flashing:** quit `make monitor` first (Ctrl+] or `pkill -f serial.tools.miniterm`).

Stuck? `/hint 01 1` gives a nudge, and levels 2 and 3 give stronger hints. When you're done, run `/review 01`.

---

## 9. Results and answers (2026-10-14)

### What I built
`workspace/apps/01_blink/src/main.c`, with `#define DEMO DEMO_SOS`. The timed blink from TODO 1 is still there as `DEMO_TIMED_BLINK`.

### Measured on the board
```
SOS demo: unit 200 ms, ideal cycle 34 units = 6800000 us
SOS #16  cycle: 6802200 us
SOS #17  cycle: 6802200 us
...                              (identical every cycle)
SOS #22  cycle: 6802200 us
```

### Exercise A answers
1. **Units per cycle:** S (3 dots + 2 gaps = 5) + O (3 dashes + 2 gaps = 11) + S (5) + 2 letter gaps (6) + word gap (7) = **34 units**.
2. **Milliseconds:** 34 × 200 ms = **6800 ms** (6,800,000 µs).
3. **Blocks:** **9 ON** blocks (3 + 3 + 3 elements) and **9 OFF** blocks (one after each element: 6 element gaps, 2 letter gaps, 1 word gap).
   The code needs **22 `k_msleep` calls** to make those blocks, because each longer gap is built from an element gap plus a "pad" sleep.

### Why the measurement is 2.2 ms longer
- Measured minus ideal = 6,802,200 − 6,800,000 = **2,200 µs = 22 ticks** (1 tick = 0.1 ms).
- `k_msleep` calls per cycle:
  - 9 × `blink()` × 2 sleeps = 18
  - 3 letter-gap pads (one per letter) = 3
  - 1 word-gap pad = 1
  - **total = 22**
- So it's **exactly +1 tick per sleep**, the same rule found in TODO 1. Each sleep lasts *at least* the requested time, so the kernel rounds up by one tick.
- The cycle is **identical every time** (no jitter at 0.1 ms resolution). GPIO calls and the one printk per cycle cost less than one tick.
- **Drift:** 2.2 ms per 6.8 s = 0.032 %, about **1.2 s per hour**. More sleep calls per cycle means more drift, so the error grows with the *number of sleeps*, not with the length of time. The fix is the stretch goal: sleep until an absolute deadline.

### My decisions
| Decision | Choice | Why |
|---|---|---|
| 1. Pattern | Helper functions (option a). `send_s`/`send_o` share one `send_letter(on_ms)` | Clear to read. A pattern table (option b) is the next step if I want any message |
| 2. Constants | `#define`, all derived from `UNIT_MS` | One number sets the speed |
| 3. Old blink | Kept, selected by `#define DEMO` | Both demos stay compiled and checked. A Kconfig option is the lesson 03 version |
| 4. Errors | Helpers return `0` or the GPIO error. `main()` prints it | A failure inside a letter isn't silently ignored |
| 5. printk | One line per cycle | Readable, and about 30 characters per 6.8 s never fills the 128-byte UART FIFO |

**`#define` vs `static const` vs `enum` (Decision 2 question):**
- `#define` is plain text substitution by the preprocessor. It has no type, no address, and normally doesn't show up in the debugger.
- `static const` is a real typed object: the compiler checks its type, and GDB can show it. But in C it isn't a true compile-time constant, so you can't use it for an array size at file scope.
- `enum` values are typed integer constants. They're usable as compile-time constants *and* visible to GDB by name.

For timing values that get multiplied together, `#define` (in parentheses) is the common Zephyr style.

### Still open
- [x] Watch it by eye: confirmed 2026-10-14. SOS clearly readable on the LED.
- [ ] Stretch goals: any-message player, no-drift version, dots and dashes on two LEDs (after TODO 3).
