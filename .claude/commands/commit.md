---
name: commit
description: Stage changes, run a local review pass, draft a Conventional Commit message from the diff, and commit. Refuses to run on master/main/dev/develop.
allowed-tools: Bash(git status), Bash(git status *), Bash(git diff), Bash(git diff *), Bash(git log), Bash(git log *), Bash(git branch *), Bash(git add *), Bash(git commit -F *), Bash(git commit -m *)
---

Create a commit on the current feature branch.

**Refuse if HEAD is `master`, `main`, `dev`, or `develop`** — tell the user to run `/branch <name>` first. (These four are protected by the pre-commit hook anyway; the slash command short-circuits before the hook runs.)

Steps:
1. Run `git status` and `git branch --show-current` in parallel.
2. If branch is `master`, `main`, `dev`, or `develop`, stop and instruct the user to create a feature branch with `/branch`.
3. Run `git diff` (unstaged) and `git diff --staged` in parallel to see all pending changes. Run `git log -n 5 --oneline` to match the repo's commit-message style.
4. Do a quick local review pass on the diff: look for obvious issues (debug prints, commented-out code, secrets, unrelated changes). Surface anything notable to the user **before** committing — let them decide whether to proceed.
5. Stage the relevant files explicitly by name (avoid `git add -A` / `git add .` — they pick up junk). Skip anything matching `.env`, `*.key`, `*.pem`, build outputs.
6. Draft a Conventional Commit message:
   - Subject: `<type>(<optional-scope>): <imperative summary>` — types are `feat`, `fix`, `chore`, `docs`, `refactor`, `test`, `perf`, `style`, `build`, `ci`. Subject ≤72 chars.
   - Blank line, then a body explaining *why* (1–3 short paragraphs, only if non-trivial).
   - Trailer: `Co-Authored-By: Claude Opus 4.7 (1M context) <noreply@anthropic.com>` (Claude Code's standard sign-off).
7. Commit with the message via HEREDOC + `git commit -F -` so multi-line formatting is preserved.
8. Confirm with `git status` and `git log -n 1`.

The pre-commit hook will run automatically — if it fails, fix the underlying issue and create a NEW commit. Do not bypass the hook with `--no-verify`.
