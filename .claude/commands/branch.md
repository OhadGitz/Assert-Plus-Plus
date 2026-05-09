---
name: branch
description: Create and switch to a feature branch from latest dev. Argument is the branch name (e.g. feat/foo, fix/bar, chore/baz, docs/qux).
allowed-tools: Bash(git fetch), Bash(git fetch *), Bash(git switch *), Bash(git checkout -b *), Bash(git status), Bash(git branch), Bash(git pull --rebase *)
---

Create a new feature branch from the latest `dev`.

This repo uses a Git Flow / Semantic Versioning Git Branching scheme:

- `master` (alias `main`) — production / release-tagged
- `dev` (alias `develop`) — integration branch; all feature work flows through here
- feature branches branch **off `dev`** and merge **back into `dev`** via PR
- periodic `dev → master` PRs cut a release

Feature branches **never** branch off `master` directly.

Branch name comes from `$ARGUMENTS`. Validate it matches the same regex enforced by the pre-commit hook:

```
^(feat|feature|fix|bugfix|hotfix|chore|docs|refactor|test|perf|style|build|ci|private|wip|experiment|spike|release)/[a-z0-9][a-z0-9._-]*$
```

If it doesn't match, stop and tell the user the expected pattern + the allowed prefix list. Do **not** create the branch — committing on it would be blocked by the hook anyway.

Steps:
1. Run `git status` — abort if working tree is dirty (uncommitted changes belong to the previous task; ask the user how to handle them).
2. Run `git fetch origin dev` to make sure local `dev` ref is current.
   - If `origin/dev` does not exist, stop and tell the user to bootstrap it once: `git switch master && git pull --rebase && git switch -c dev && git push -u origin dev`. Do **not** auto-create `dev`.
3. Run `git switch dev && git pull --rebase origin dev` to update local `dev`.
4. Run `git checkout -b $ARGUMENTS` to create and switch to the new branch.
5. Confirm with `git status` and report the new branch name + the `dev` SHA it was based on.
