#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
test_build="${DINNER_UI_BUILD:-/tmp/dinner-ui-build}"
cmake -S "${repo_root}/tests/dinner_ui" -B "${test_build}"
cmake --build "${test_build}" --parallel 6
mkdir -p "${repo_root}/build/preview"
cd "${repo_root}/build/preview"
"${test_build}/dinner_ui_test"
