# AGENTS.md

C11 codebase (Linux task manager, developed on macOS/LLVM). No Make/CMake/CI —
builds are plain `clang` invocations in `scripts/`.

## Commands
- `./.utils/build.sh` — build all 3 binaries into `build/`
- `./.utils/test.sh`   — run `build/wetman_test` (all unit tests)
- `./.utils/run.sh`    — run `build/wetman_server`
- All builds use `clang -std=c11 -pedantic -Wall -Wextra -I ./src`

## Build architecture (important)
Each binary is a single translation unit: `main.c` `#include`s `.mod.c` files
that transitively `#include` every `.c` file (guarded by `WETMAN_*_MOD_C`).
- A new `.c`/`.h` file is NOT compiled unless you `#include` it from its
  directory's `mod.c` (create a `mod.c` for new module dirs, then include it
  from the parent `mod.c` or `main.c`). Forgetting this silently excludes code
  from all binaries.
- Library headers are included as `<wetman/...>`; entrypoints:
  `src/wetman/server/main.c`, `src/wetman/client/main.c`, `test/main.c`.

## Tests
- Harness in `src/wetman/utils/test/`. Define tests with `TEST(Name)`; register
  via `REGISTER_TEST(Name)` in the matching `register*Tests()` in
  `test/.../mod.c`, chained up to `test/main.c`'s `registerUtilTests()` /
  `registerServerTests()`.
- New test file: add `#include "ut_x.c"` to the matching `test/.../mod.c`.
- `ASSERT_*` macros abort the test on failure; `EXPECT_*` continue.
- Tests run inside `.test_wdir/` (gitignored). `CREATE_TMP_FILE(flags)` creates
  files under `tmp_files/`; cleanup is disabled, so they persist after a run.

## Conventions
- Functions are `Module_Action` (e.g. `Arena_New`, `EndpointRegistry_CallEndpoint`).
- Use aliases from `src/wetman/utils/type.h` (`i8..i64`, `u8..u64`, `usize`,
  `isize`) and macros from `macro.h` (`TRUE/FALSE`, `MAX/MIN`, `LIKELY/UNLIKELY`,
  `MAYBE_UNUSED`).
- Server listens on Unix socket `/tmp/wetman_server.sock`; the client connects
  to it. Endpoint IDs are plain constants in each `main.c`.
