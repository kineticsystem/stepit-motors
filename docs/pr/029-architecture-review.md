# A Second Check of the Architecture, and a Review

Pull request [#29](https://github.com/kineticsystem/stepit-motors/pull/29), branch `architecture-review`.

## Goal

A second, independent check of the documents, by a stronger model than the one that wrote them: every claim of the architecture document read against the code, and a design review with its suspicions proved by throwaway tests.

This pull request changes documents only. No code, no test and no configuration changes.

## Decisions

- **The architecture document is corrected, not rewritten.** What was wrong: the update rate in the big-picture diagram (100 Hz, not 30), the number of requests of a `write()`, what the fake driver validates, what `ros2_control` does after an `ERROR` (it moves the hardware to `unconfigured`, measured on 4.48.1). What was missing: Threads, the lifecycle callbacks that do not exist, the differences between the fake and the board.
- **`docs/Review.md` is new.** Its verdict: the design is sound and no path leaves a motor turning. Its three weaknesses: the handshake after a stale reply, a failed cycle that ends the session, and fake motors that do not behave like the board.

- **The review fixes nothing.** Each finding says whether a test proved it or it comes from reading, and whether it is a safety issue for the rig. The fixes are separate pull requests, in the order of the review's recommendations.
- **The throwaway tests are not kept.** They ran in the image of StepIt Macro, on a scratch copy of the packages, and were removed. A finding worth a permanent test gets one with its fix.
- **The architecture document describes the design and holds no review.** Where a probe of the review disproved a sentence of it, the sentence is corrected and points to `Review.md`.

## Open Items

- A Move truncates its target instead of rounding: one position in eight loses a step. See the review.
- Nothing refuses a NaN or an infinite position, on the host or in the firmware.
- `CLAUDE.md` says C++20; the CMake files set C++23.
