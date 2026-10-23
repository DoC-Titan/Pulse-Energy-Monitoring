#!/usr/bin/env bash
#
# Publish docs/ to the gh-pages branch, which is what GitHub Pages deploys.
#
#   ./publish.sh            # build and push
#   ./publish.sh --dry-run  # show what would be published, no push
#
# GitHub Pages on this repository deploys from the `gh-pages` branch via
# GitHub Actions. The `github-pages` environment only allows deployments
# from that branch, so main/docs is published through this script.
#
set -euo pipefail

cd "$(dirname "$0")"

REPO_ROOT="$(pwd)"
REMOTE="${REMOTE:-origin}"
BRANCH="gh-pages"
DRY_RUN=0

for arg in "$@"; do
  case "$arg" in
    --dry-run) DRY_RUN=1 ;;
    -h|--help) sed -n '2,12p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) echo "Unknown argument: $arg" >&2; exit 2 ;;
  esac
done

if [ ! -f docs/index.html ]; then
  echo "error: docs/index.html not found; run this from the repository root." >&2
  exit 1
fi

if ! git remote get-url "$REMOTE" >/dev/null 2>&1; then
  echo "error: git remote '$REMOTE' is not configured." >&2
  exit 1
fi

AUTHOR_NAME="$(git config user.name  || echo 'Pulse Bot')"
AUTHOR_EMAIL="$(git config user.email || echo 'pulse@localhost')"

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# 1. Site files come from docs/
cp -a docs/. "$WORK"/
# Guarantee .nojekyll so assets are served verbatim.
: > "$WORK/.nojekyll"

# 2. Carry the deploy workflow, which must exist on gh-pages to run there.
mkdir -p "$WORK/.github/workflows"
cp .github/pages-workflow.yml "$WORK/.github/workflows/deploy.yml"

echo "Publishing $(find "$WORK" -type f -not -path '*/.git/*' | wc -l) files from docs/ to '$BRANCH'"

if [ "$DRY_RUN" -eq 1 ]; then
  echo "--- dry run: contents of $BRANCH ---"
  (cd "$WORK" && find . -type f -not -path './.git/*' | sort)
  exit 0
fi

# 3. Rebuild the branch as a single commit and force-push it.
cd "$WORK"
git init -q -b "$BRANCH"
git add -A
git -c user.name="$AUTHOR_NAME" -c user.email="$AUTHOR_EMAIL" \
    commit -q -m "Publish site from docs/ ($(date -u +%Y-%m-%dT%H:%M:%SZ))"
git push -q --force "$REPO_ROOT" "HEAD:$BRANCH"

echo "Pushed to $BRANCH. GitHub Actions is deploying now:"
echo "  https://github.com/$(git -C "$REPO_ROOT" remote get-url "$REMOTE" \
      | sed -E 's#.*github\.com[:/]([^/]+/[^/.]+)(\.git)?.*#\1#')/actions"
echo "Live site (allow ~1 minute):"
echo "  https://$(git -C "$REPO_ROOT" remote get-url "$REMOTE" \
      | sed -E 's#.*github\.com[:/]([^/]+)/.*#\1#').github.io/$(git -C "$REPO_ROOT" remote get-url "$REMOTE" \
      | sed -E 's#.*github\.com[:/][^/]+/([^/.]+)(\.git)?.*#\1#')/"
