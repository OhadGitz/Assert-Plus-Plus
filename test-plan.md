# Test plan — testing environment design (deferred)

> This is a captured design for the testing environment. Implementation is deferred to a later branch; the document exists so the work isn't lost.
>
> Context: the original `Assertive/Assertive.cpp` `main()` is a smoke demo of failure output, not a test suite. This plan describes what a proper test suite for the library would look like.

## Requirements

### R1 — Expectation semantics

Every test declares one of two expectations about what happens when the code under test runs:

| Declared expectation       | Pass condition                 | Fail condition       |
|----------------------------|--------------------------------|----------------------|
| expect at least one assert | ≥1 assertion fired in the body | 0 assertions fired   |
| expect no asserts          | 0 assertions fired in the body | ≥1 assertion fired   |

Both directions are first-class failure modes. There is no "don't care" expectation.

### R2 — Each test in its own function

Each test case lives in its own dedicated function/scope (not a sequence of macros inside one `main()`). Target framework: **Catch2** — each test is a `TEST_CASE`.

### R3 — Verify the formatted assertion output text

Tests must assert on the **text** the library prints for a failed assertion, not just the fact that it fired. The library prints three lines per fire:

```
assertion '1 + 2 == 2' failed
  in <file>:<line> (<function>)
  expansion: 3 == 2
```

Both the **source form** (`'1 + 2 == 2'`) and the **expanded form** (`3 == 2`) must be verifiable from tests.

**Match strictness:** substring matching. Tests assert that the captured text *contains* the expected expression text and the expected expansion text. The `in <file>:<line>` middle line is ignored, so tests aren't broken by edits that shift line numbers above an `ASSERT`.

## Design implications

Given R1+R3, the library needs:

1. A **swappable output sink** for `on_assert_failed` — default sink = `std::cout` (production behavior unchanged); tests install a capturing sink.
2. The sink / test-side wrapper must expose **both** the fire count and the accumulated text, because R1 needs the count and R3 needs the text.

This rules out an exception-based "throw on fire" design: exceptions terminate the body at the first fire, so they can't satisfy R1 ("≥1 fire" / count semantics) when combined with R3 (per-fire text capture) in one test body.

Likely shape (subject to revision when the test branch is opened):

- `Assertive.h` exposes a sink registration call. Default sink prints to stdout (matches today).
- `tests/` uses Catch2; an RAII helper `capture_assertions` swaps the sink for the duration of a `TEST_CASE`, restores on destruction, and offers `.fires()` (count) and `.text()` (captured stream).
- Tests use Catch2's `REQUIRE` + `REQUIRE_THAT` with `ContainsSubstring` matchers, or thin `REQUIRE_ASSERTS(stmt)` / `REQUIRE_NO_ASSERTS(stmt)` macros built on top — TBD when the work starts.

## Open items (resolve when test work begins)

- Exact sink API shape (callback vs `std::ostream*` vs structured record).
- Catch2 version and how to obtain it (FetchContent, system package, vendored).
- CMake / build wiring.
- Test layout (single file vs split by operator family).
- Whether to convert `Assertive/` from an executable to a library at that point.
