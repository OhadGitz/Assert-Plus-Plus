# CLAUDE.md

Project guidelines for Claude Code working in this repo.

> **Status:** workflow rules only (Phase 0). Build/test commands, C++ conventions, and project overview land in subsequent phases.

---

## Git workflow (non-negotiable)

### Branching

- **Never commit on `master`, `main`, `dev`, or `develop`.** The pre-commit hook blocks this.
- Always work on a feature branch. Create one with the `/branch` slash command, or:
  ```bash
  git checkout -b <type>/<short-kebab-name>
  ```
- Branch names must match `<type>/<name>`. Allowed types:
  - `feat` / `feature` — new feature
  - `fix` / `bugfix` / `hotfix` — bug fix
  - `chore` — tooling, config, housekeeping
  - `docs` — documentation
  - `refactor` — internal restructuring, no behavior change
  - `test` — tests only
  - `perf` — performance work
  - `style` — formatting / lint only
  - `build` — build system / dependencies
  - `ci` — CI config
  - `private` / `wip` — personal / unfinished (not for PR)
  - `experiment` / `spike` — exploratory work
  - `release` — release branches
- Names are lowercase kebab-case. Allowed chars: `[a-z0-9._-]`.

### Commits

- One logical change per commit. Atomic.
- **Conventional Commits** message format:
  ```
  <type>(<optional-scope>): <imperative summary ≤72 chars>

  <optional body explaining WHY, not WHAT>

  Co-Authored-By: Claude Opus 4.7 (1M context) <noreply@anthropic.com>
  ```
- Types: `feat`, `fix`, `chore`, `docs`, `refactor`, `test`, `perf`, `style`, `build`, `ci`.
- Stage files explicitly by name. **Do not** use `git add -A` or `git add .` — they pick up junk.
- **Do not** bypass the pre-commit hook with `--no-verify`. If the hook fails, fix the underlying issue and create a NEW commit.
- **Do not** amend published commits. Create a new commit instead.

### Pull requests

- Every change reaches `master` via a PR. No direct merges.
- Open PRs with the `/pr` slash command (handles push + `gh pr create`).
- PR title: ≤70 chars, Conventional Commit style.
- PR body: `## Summary` (1–3 bullets, why), `## Test plan` (concrete checklist).
- Don't push to a PR branch with `--force`. If a rebase is needed, run `/sync` and resolve conflicts locally.

### Reviewing your own work

- Before `/commit`: review the diff for debug prints, commented-out code, secrets, unrelated changes.
- Before `/pr`: re-read all commits since `master` (`git log master..HEAD`), not just the latest.

---

## Pre-commit hook

A pre-commit hook lives at `tools/hooks/pre-commit`. Install once per clone:

```bash
./tools/install-hooks.sh
```

It enforces, in order:

1. Refuses commits on `master` / `main` / `dev` / `develop`.
2. Refuses commits on branches whose name doesn't match the pattern above.
3. Runs `clang-format --dry-run --Werror` on staged C++ files.
4. Runs `cmake --build build` if a `build/` directory exists.

If a gate fails, fix the cause and re-stage. Never bypass with `--no-verify`.

---

## Slash commands available in this repo

| Command  | What it does |
|----------|-----------------------------------------------------------|
| `/branch <name>` | Create + switch to a feature branch from latest `master`. |
| `/commit`        | Stage, review the diff, draft a Conventional Commit, commit. |
| `/pr`            | Push the branch and open a PR with a generated title + body. |
| `/sync`          | Fetch + rebase the current branch on `origin/master`.       |

See `.claude/commands/` for the full prompts.

---

## Permissions deliberately *not* allowlisted

These require explicit user approval each time — do not work around them:

- `git push --force` / `git push -f`
- `git reset --hard`
- `git clean -fd`
- `git commit --no-verify`

If you find yourself reaching for one of these, stop and tell the user what you'd do and why, then wait for them to authorize it.
