---
name: pr
description: Push the current branch and open a GitHub PR. Asks which base to target — defaults to `dev` for feature work, `master` for release PRs.
allowed-tools: Bash(git status), Bash(git branch *), Bash(git log *), Bash(git diff *), Bash(git push -u origin *), Bash(git push origin HEAD), Bash(gh pr create *), Bash(gh pr view *), Bash(gh repo view *)
---

Push the current branch and open a PR.

This repo uses Git Flow / Semantic Versioning Git Branching:

- **Feature PRs** target `dev` (default).
- **Release PRs** target `master` — only when promoting accumulated work from `dev → master` (typically the head branch is `dev` itself, or a `release/*` branch cut from `dev`).

**Refuse if HEAD is `master`, `main`, or `develop`.** (`dev` is allowed only when explicitly cutting a release.)

Steps:
1. Run `git status`, `git branch --show-current`, `gh repo view --json nameWithOwner` in parallel.
2. If the current branch is `master`/`main`/`develop`, stop. If it's `dev`, only continue when the user has confirmed this is a release PR (next step).
3. **Ask the user which branch this PR should target.** Default: `dev`. Other accepted answer: `master` (release PR). Capture the answer as `$BASE`. If `$BASE` is `master` and HEAD is a normal feature branch, double-check with the user — feature branches almost always target `dev`.
4. Run `git log $BASE..HEAD --oneline` and `git diff $BASE...HEAD` in parallel to understand the full set of changes since branching from `$BASE`. **Look at all commits, not just the latest.**
5. Push: `git push -u origin HEAD`. If a non-fast-forward push is needed, stop and ask the user — never use `--force`.
6. Draft the PR:
   - **Title** (≤70 chars, matches the dominant commit type): e.g. `feat(commands): add /branch /commit /pr slash commands`. For a release PR use `chore(release): dev → master (vX.Y.Z)` style.
   - **Body** (HEREDOC):
     ```
     ## Summary
     - 1–3 bullets of what this PR does and why.

     ## Test plan
     - [ ] Concrete checklist a reviewer can run to verify.

     🤖 Generated with [Claude Code](https://claude.com/claude-code)
     ```
7. Run `gh pr create --base "$BASE" --title "..." --body "$(cat <<'EOF' ... EOF)"`. Default to a regular PR (not draft) unless the user says draft.
8. Print the PR URL.
