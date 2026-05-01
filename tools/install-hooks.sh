#!/usr/bin/env bash
# Install the project's git hooks into .git/hooks.
# Run once per clone:  ./tools/install-hooks.sh

set -euo pipefail

repo_root="$(git rev-parse --show-toplevel)"
src_dir="$repo_root/tools/hooks"
dest_dir="$repo_root/.git/hooks"

if [[ ! -d "$src_dir" ]]; then
    echo "install-hooks: $src_dir not found." >&2
    exit 1
fi

mkdir -p "$dest_dir"

for hook in "$src_dir"/*; do
    [[ -f "$hook" ]] || continue
    name="$(basename "$hook")"
    install -m 0755 "$hook" "$dest_dir/$name"
    echo "installed: $dest_dir/$name"
done

echo "done."
