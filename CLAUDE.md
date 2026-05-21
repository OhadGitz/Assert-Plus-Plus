# CLAUDE.md

Project guidelines for Claude Code working in this repo.

> **Status:** workflow rules only (Phase 0). Build/test commands, C++ conventions, and project overview land in subsequent phases.

---

## Git workflow (non-negotiable)

### Branching model: Git Flow / Semantic Versioning Git Branching

Two long-lived branches:

- **`master`** (alias `main`) — production / release-tagged. Never commit directly.
- **`dev`** (alias `develop`) — integration branch where all feature work converges. Never commit directly.

Flow:

```
        feat/x ──┐
                 ▼
        feat/y ──► dev ────────────► master   (release PR)
                 ▲     (every few           ▲
        fix/z ───┘      features)           │
                                            └── (tag vX.Y.Z here)
```

- Feature branches **branch off `dev`** and merge back into `dev` via PR. They never branch off `master`.
- Promotion to `master` happens periodically as a single `dev → master` release PR. SemVer tags are cut from `master`.
- Hotfixes (`hotfix/*`) may branch off `master` directly when production is broken; they merge to **both** `master` and `dev`.

### Branching rules

- **Never commit on `master`, `main`, `dev`, or `develop`.** The pre-commit hook blocks this.
- Always work on a feature branch off `dev`. Create one with the `/branch` slash command, or:
  ```bash
  git fetch origin dev
  git switch dev && git pull --rebase origin dev
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

- Every change reaches `dev` (and ultimately `master`) via a PR. No direct merges.
- **Default target: `dev`.** Feature, fix, chore, docs, refactor, test, perf, style, build, ci branches all PR into `dev`.
- **Release PRs target `master`.** Source is usually `dev` itself or a `release/*` branch cut from `dev`. Title style: `chore(release): dev → master (vX.Y.Z)`.
- Hotfix PRs target `master` directly; a follow-up PR merges the hotfix back into `dev` so it doesn't get lost on the next release.
- Open PRs with the `/pr` slash command — it asks for the target branch (default `dev`) and handles push + `gh pr create --base <target>`.
- PR title: ≤70 chars, Conventional Commit style.
- PR body: `## Summary` (1–3 bullets, why), `## Test plan` (concrete checklist).
- Don't push to a PR branch with `--force`. If a rebase is needed, run `/sync` and resolve conflicts locally.

### Reviewing your own work

- Before `/commit`: review the diff for debug prints, commented-out code, secrets, unrelated changes.
- Before `/pr`: re-read all commits since the PR base (`git log <base>..HEAD` — usually `dev..HEAD`), not just the latest.

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
| `/branch <name>` | Create + switch to a feature branch from latest `dev`. |
| `/commit`        | Stage, review the diff, draft a Conventional Commit, commit. |
| `/pr`            | Push the branch and open a PR; asks for target (default `dev`). |
| `/sync`          | Fetch + rebase the current branch on `origin/dev` (or `origin/master` for release PRs). |

See `.claude/commands/` for the full prompts.

---

## Permissions deliberately *not* allowlisted

These require explicit user approval each time — do not work around them:

- `git push --force` / `git push -f`
- `git reset --hard`
- `git clean -fd`
- `git commit --no-verify`

If you find yourself reaching for one of these, stop and tell the user what you'd do and why, then wait for them to authorize it.

## Merging is the user's job — never Claude's

**Claude must never merge a PR or a branch.** Merging is a shared-state action that the user performs via the GitHub UI (or by explicitly running the merge command themselves).

This applies to all forms of merge, including:

- `gh pr merge` (any flags, any merge strategy)
- `git merge` (local merges)
- REST-API workarounds via `gh api .../merges` or `gh api .../pulls/N/merge`

These are denied in `.claude/settings.json` so the tool calls are blocked at the harness level. Even if a deny rule is missing, do not propose to merge — prepare the PR (refresh body, verify checks, resolve conflicts via `/sync`) and hand off to the user.
