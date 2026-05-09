---
name: sync
description: Fetch origin and rebase the current feature branch on the latest dev. For release PRs, rebases on master instead. Aborts on conflicts so the user can resolve.
allowed-tools: Bash(git status), Bash(git branch *), Bash(git fetch), Bash(git fetch *), Bash(git rebase dev), Bash(git rebase origin/dev), Bash(git rebase master), Bash(git rebase origin/master), Bash(git rebase --abort)
---

Fetch and rebase the current branch on the latest integration branch.

Default integration branch is `dev`. The exception is a release PR (`dev → master`), which rebases on `master`.

**Refuse if HEAD is `master`, `main`, `dev`, or `develop`** (no rebase to do on protected branches).

Steps:
1. Run `git status` and `git branch --show-current` in parallel. Abort if working tree is dirty.
2. Pick the rebase target:
   - Default: `origin/dev`.
   - If the current branch is `release/*` or the user explicitly says this is a release PR, use `origin/master`.
3. Run `git fetch origin <target>`.
4. Run `git rebase origin/<target>`.
5. If conflicts: run `git rebase --abort` and tell the user the rebase failed — don't try to resolve conflicts unattended.
6. On success, report the new HEAD with `git log -n 1 --oneline`.
