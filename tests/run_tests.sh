#!/usr/bin/env bash
# Regression tests for issue #4: bounds-check on graph file inputs.
# Requires: gcc, python3
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$SCRIPT_DIR/.."
VALIDATOR="$ROOT/dist/validate_file"
TMPDIR_TESTS="$(mktemp -d)"
trap 'rm -rf "$TMPDIR_TESTS"' EXIT

PASS=0; FAIL=0

pass() { echo "[PASS] $1"; PASS=$((PASS + 1)); }
fail() { echo "[FAIL] $1"; FAIL=$((FAIL + 1)); }

# Build the standalone validator
echo "Building validator..."
gcc -std=c17 -Wall -g "$SCRIPT_DIR/validate_file.c" -o "$VALIDATOR"
echo "Built: $VALIDATOR"
echo

# Helper: write a binary graph file via Python.
# Args: path numVertices numEdges src dst
# Header layout: unsigned long (4B) + 4B padding + unsigned long long (8B) = 16B
# EdgeRecord layout: unsigned long (4B) + unsigned long (4B) = 8B
write_graph() {
    local path="$1" numV="$2" numE="$3" src="$4" dst="$5"
    python3 - "$path" "$numV" "$numE" "$src" "$dst" <<'PYEOF'
import sys, struct
path, numV, numE, src, dst = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
with open(path, 'wb') as f:
    # Linux 64-bit: unsigned long = 8B, unsigned long long = 8B, no padding
    # header: QQ (8B numVertices + 8B numEdges) = 16 bytes
    f.write(struct.pack('<QQ', numV, numE))
    # edgerecord: QQ (8B source + 8B destination) = 16 bytes each
    for _ in range(numE):
        f.write(struct.pack('<QQ', src, dst))
PYEOF
}

MAXV=1000000

# ---- Test 1: valid graph ----
G="$TMPDIR_TESTS/valid.graph"
write_graph "$G" 8 2 3 5
if "$VALIDATOR" "$G"; then
    pass "valid graph (numV=8, 2 edges, all < MAXV)"
else
    fail "valid graph should succeed but exited $?"
fi

# ---- Test 2: numVertices > MAXV ----
G="$TMPDIR_TESTS/oob_header.graph"
write_graph "$G" $((MAXV + 1)) 0 0 0
set +e
OUT=$("$VALIDATOR" "$G" 2>&1); RC=$?
set -e
if [[ $RC -ne 0 ]] && echo "$OUT" | grep -q "exceeds MAXV"; then
    pass "OOB header: numVertices = MAXV+1 → rejected with correct message"
else
    fail "OOB header: expected rejection (rc=$RC, out='$OUT')"
fi

# ---- Test 3: edge source > MAXV ----
G="$TMPDIR_TESTS/oob_src.graph"
write_graph "$G" 8 1 $((MAXV + 1)) 3
set +e
OUT=$("$VALIDATOR" "$G" 2>&1); RC=$?
set -e
if [[ $RC -ne 0 ]] && echo "$OUT" | grep -q "out of bounds"; then
    pass "OOB edge source = MAXV+1 → rejected with correct message"
else
    fail "OOB edge source: expected rejection (rc=$RC, out='$OUT')"
fi

# ---- Test 4: edge destination > MAXV ----
G="$TMPDIR_TESTS/oob_dst.graph"
write_graph "$G" 8 1 3 $((MAXV + 1))
set +e
OUT=$("$VALIDATOR" "$G" 2>&1); RC=$?
set -e
if [[ $RC -ne 0 ]] && echo "$OUT" | grep -q "out of bounds"; then
    pass "OOB edge destination = MAXV+1 → rejected with correct message"
else
    fail "OOB edge destination: expected rejection (rc=$RC, out='$OUT')"
fi

# ---- Test 5: boundary value — vertex exactly at MAXV (should succeed) ----
G="$TMPDIR_TESTS/boundary.graph"
write_graph "$G" $MAXV 1 $((MAXV - 1)) $((MAXV - 1))
if "$VALIDATOR" "$G"; then
    pass "boundary: vertex = MAXV-1 (within bounds) → accepted"
else
    fail "boundary: vertex at MAXV-1 should be accepted but was rejected"
fi

echo
echo "Results: $PASS passed, $FAIL failed out of $((PASS + FAIL)) tests"
[[ $FAIL -eq 0 ]]
