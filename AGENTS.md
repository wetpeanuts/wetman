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
  The client binary also includes `shared/mod.c` (for persistence APIs like
  `WorkspaceConfig_Read` used in CLI commands).
- Endpoints are split by namespace: `shared/endpoint/` holds the interface
  (`Endpoint_<Name>_Request`/`_Response` structs + inline serializers), while
  `server/endpoint/` defines server logic + `ENDPOINT_DECLARE_SERVER` +
  `ENDPOINT_IMPL_SERVER` and `client/endpoint/` adds `ENDPOINT_DECLARE_CLIENT` +
  `ENDPOINT_IMPL_CLIENT`. The client binary must include the shared +
  client endpoint `mod.c` trees to see the serializers.
- An endpoint only works after it is registered in `server/main.c`
  (`EndpointRegistry_RegisterEndpoint(&reg, Endpoint_<Name>_Create())`);
  skipping registration compiles fine but clients get
  `RETURN_CODE_INVALID_ENDPOINT_ID`.
- The client binary is a CLI built around a command-parser system:
  `ClientContext` (`client/context.[ch]`) holds a global `Arena` and `Client`.
  Commands live in `client/commands/` — each command is a `Command` struct
  (`commands/command.h`) with a name, arg spec, and handler. Registration is
  in `client/main.c` via `CommandParser_RegisterCommand(&parser,
  Command_<Name>_Create(&arena))`. Available commands: `healthcheck`,
  `workspace init [-n|--name <name>]`, `workspace delete [-w|--workspace <id>]`,
  `workspace list`, `-h/--help`. Adding a command = create
  `commands/<name>.c` + `.h`, add `#include` to `commands/mod.c`, and register
  in `main.c`.
- Request deserialization: `RequestDeserializer_Deserialize(req, arena)` and
  response deserialization: `ResponseDeserializer_Deserialize(resp, arena)`
  both accept an `Arena*` to allocate any resources owned by the
  request/response (e.g. slices, strings).
- Global server state lives in `ServerContext` (`server/context.[ch]`), set up
  via `ServerContext_Init(arena, wdir)` from `server/main.c`; workspaces are
  stored under `<wdir>/workspaces/<id>` and the id counter persists in
  `<wdir>/next_workspace_id`.
- Client transports live in `utils/net/client/`: `UnixClient_Connect(socketPath)`
  for unix sockets and `EndpointClient_Connect(registry)` for in-process calls.
  They return a `Client` by value; call `client.disconnect(&client)` to release
  the client's arena (it is not freed otherwise).

## Tests
- Harness in `src/wetman/utils/test/`. Define tests with `TEST(Name)`; register
  via `REGISTER_TEST(Name)` in the matching `register*Tests()` in
  `test/.../mod.c`, chained up to `test/main.c`'s `registerUtilTests()` /
  `registerServerTests()`.
- New test file: add `#include "ut_x.c"` to the matching `test/.../mod.c`.
- Shared test endpoints (e.g. `echo_i32`, `echo_str`) live in
  `test/shared/endpoint/`: structs + inline serializers in `echo_x.h`, server
  impl + `ENDPOINT_IMPL_SERVER(id, ...)` in `echo_x.c`, ids in `id.h`. Each
  `echo_x.c` must be added to `mod.c`, and `test/main.c` includes
  `shared/mod.c` before the utils/server test trees. To call one from a test,
  generate the client-side `ENDPOINT_IMPL_CLIENT(id, Name)` in the test file.
- `ASSERT_*` macros abort the test on failure; `EXPECT_*` continue.
- Tests run inside `.test_wdir/` (gitignored). `CREATE_TMP_FILE(flags)` creates
  files under `tmp_files/` and `CREATE_TMP_DIR(buf)` makes a fresh directory
  under `tmp_dirs/`, writing its path into `buf`; cleanup is disabled, so they
  persist after a run.
- Data structure tests live in `test/utils/data_struct/` (e.g. `ut_slice_i32.c`,
  `ut_slice_str.c`, `ut_slice_i64.c`, `ut_slice_u32.c`, `ut_slice_u64.c`).
- Serialization tests live in `test/utils/ut_data_stream.c`.

## Conventions
- Functions are `Module_Action` (e.g. `Arena_New`, `EndpointRegistry_CallEndpoint`);
  filesystem helpers use the `FS_` prefix (`filesystem.h`: `FS_PathJoin`,
  `FS_CreateDir`, `FS_OpenFile`, ...) with paths passed as `Str`.
- Use aliases from `src/wetman/utils/type.h` (`i8..i64`, `u8..u64`, `usize`,
  `isize`) and macros from `macro.h` (`TRUE/FALSE`, `MAX/MIN`, `LIKELY/UNLIKELY`,
  `MAYBE_UNUSED`).
- Server listens on Unix socket `/tmp/wetman_server.sock`; the client connects
  to it. Endpoint IDs are `ENDPOINT_ID_*` constants in
  `src/wetman/shared/endpoint/id.h`, shared by both server and client.

## Data structures & serialization
- `Slice<T>` is the generic array type (`utils/data_struct/slice.h`) —
  concrete types: `SliceI32`, `SliceStr`, `SliceI64`, `SliceU32`, `SliceU64`.
  Each lives in its own `slice_x.c`/`.h` file under `utils/data_struct/`.
  Slice allocation always goes through `Arena` (no manual free).
- `DataStream` (`utils/data_stream.h`) provides `Serialize`/`Deserialize`
  helpers for every `Slice` variant (e.g. `DataStream_SerializeSliceStr`,
  `DataStream_DeserializeSliceU64`). Binary wire format: element count (u64)
  followed by raw elements (i32/u32/u64) or length-prefixed bytes (Str).
- Shared endpoint headers (`shared/endpoint/*.h`) use `DataStream` inline
  `Serialize`/`Deserialize` functions in request/response structs. The
  serialized payload is a contiguous byte buffer; endpoints never use
  separate fields for wire data — they embed `DataStream` members directly.
