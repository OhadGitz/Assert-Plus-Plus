---
name: sync
description: Fetch origin and rebase the current feature branch on the latest master. Aborts on conflicts so the user can resolve.
allowed-tools: Bash(git status), Bash(git branch *), Bash(git fetch), Bash(git fetch *), Bash(git rebase master), Bash(git rebase origin/master), Bash(git rebase --abort)
---

Fetch and rebase the current branch on the latest `master`.

**Refuse if HEAD is `master`/`main`** (no rebase to do).

Steps:
1. Run `git status` and `git branch --show-current` in parallel. Abort if working tree is dirty.
2. Run `git fetch origin master`.
3. Run `git rebase origin/master`.
4. If conflicts: run `git rebase --abort` and tell the user the rebase failed — don't try to resolve conflicts unattended.
5. On success, report the new HEAD with `git log -n 1 --oneline`.
