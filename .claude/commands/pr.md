---
name: pr
description: Push the current branch and open a GitHub PR with a generated title (Conventional Commit style) and a Summary + Test plan body.
allowed-tools: Bash(git status), Bash(git branch *), Bash(git log *), Bash(git diff *), Bash(git push -u origin *), Bash(git push origin HEAD), Bash(gh pr create *), Bash(gh pr view *), Bash(gh repo view *)
---

Push the current branch and open a PR.

**Refuse if HEAD is `master` or `main`.**

Steps:
1. Run `git status`, `git branch --show-current`, `gh repo view --json nameWithOwner` in parallel.
2. Confirm the current branch isn't `master`/`main`. If it is, stop.
3. Run `git log master..HEAD --oneline` and `git diff master...HEAD` in parallel to understand the full set of changes since branching from master. **Look at all commits, not just the latest.**
4. Push: `git push -u origin HEAD`. If a non-fast-forward push is needed, stop and ask the user — never use `--force`.
5. Draft the PR:
   - **Title** (≤70 chars, matches the dominant commit type): e.g. `feat(commands): add /branch /commit /pr slash commands`.
   - **Body** (HEREDOC):
     ```
     ## Summary
     - 1–3 bullets of what this PR does and why.

     ## Test plan
     - [ ] Concrete checklist a reviewer can run to verify.

     🤖 Generated with [Claude Code](https://claude.com/claude-code)
     ```
6. Run `gh pr create --title "..." --body "$(cat <<'EOF' ... EOF)"`. Default to a regular PR (not draft) unless the user says draft.
7. Print the PR URL.
