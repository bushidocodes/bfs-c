#!/usr/bin/env bash
# Regression tests for graph file validation (issue #4) and queue reset (issue #12).
# Requires: gcc, python3
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$SCRIPT_DIR/.."
VALIDATOR="$ROOT/dist/validate_file"
QUEUE_TEST="$ROOT/dist/test_queue"
BITARRAY_TEST="$ROOT/dist/test_bitarray"
TMPDIR_TESTS="$(mktemp -d)"
trap 'rm -rf "$TMPDIR_TESTS"' EXIT

PASS=0; FAIL=0

pass() { echo "[PASS] $1"; PASS=$((PASS + 1)); }
fail() { echo "[FAIL] $1"; FAIL=$((FAIL + 1)); }

# Build the standalone validator
echo "Building validator..."
gcc -std=c17 -Wall -g "$SCRIPT_DIR/validate_file.c" -o "$VALIDATOR"
echo "Built: $VALIDATOR"

# Build the queue unit test (with AddressSanitizer to catch OOB writes)
echo "Building queue test..."
gcc -std=c17 -Wall -g -fsanitize=address,leak \
    -I"$ROOT/src" \
    "$SCRIPT_DIR/test_queue.c" "$ROOT/src/queue.c" \
    -o "$QUEUE_TEST"
echo "Built: $QUEUE_TEST"

# Build the bitarray unit test (with AddressSanitizer to catch OOB writes)
echo "Building bitarray test..."
gcc -std=c17 -Wall -g -fsanitize=address,leak \
    -I"$ROOT/src" \
    "$SCRIPT_DIR/test_bitarray.c" "$ROOT/src/bitarray.c" \
    -o "$BITARRAY_TEST"
echo "Built: $BITARRAY_TEST"
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

# ---- Queue unit tests (issue #12) ----
set +e
QUEUE_OUT=$("$QUEUE_TEST" 2>&1); QUEUE_RC=$?
set -e
while IFS= read -r line; do
    case "$line" in
        \[PASS\]*) pass "${line#\[PASS\] }" ;;
        \[FAIL\]*) fail "${line#\[FAIL\] }" ;;
    esac
done <<< "$QUEUE_OUT"
if [[ $QUEUE_RC -ne 0 && ! "$QUEUE_OUT" == *"[FAIL]"* ]]; then
    fail "test_queue crashed or ASAN detected a violation (exit=$QUEUE_RC)"
fi

# ---- Bitarray unit tests (issue #15) ----
set +e
BITARRAY_OUT=$("$BITARRAY_TEST" 2>&1); BITARRAY_RC=$?
set -e
while IFS= read -r line; do
    case "$line" in
        \[PASS\]*) pass "${line#\[PASS\] }" ;;
        \[FAIL\]*) fail "${line#\[FAIL\] }" ;;
    esac
done <<< "$BITARRAY_OUT"
if [[ $BITARRAY_RC -ne 0 && ! "$BITARRAY_OUT" == *"[FAIL]"* ]]; then
    fail "test_bitarray crashed or ASAN detected a violation (exit=$BITARRAY_RC)"
fi

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

# ---- Test 5: truncated header (file too short to hold header struct) ----
G="$TMPDIR_TESTS/truncated_header.graph"
printf '\x01\x00' > "$G"   # only 2 bytes, well short of 16-byte header
set +e
OUT=$("$VALIDATOR" "$G" 2>&1); RC=$?
set -e
if [[ $RC -ne 0 ]] && echo "$OUT" | grep -q "failed to read header"; then
    pass "truncated header → rejected with correct message"
else
    fail "truncated header: expected rejection (rc=$RC, out='$OUT')"
fi

# ---- Test 6: truncated edge records (header says 2 edges but file only has 1) ----
G="$TMPDIR_TESTS/truncated_edges.graph"
python3 - "$G" <<'PYEOF'
import sys, struct
path = sys.argv[1]
with open(path, 'wb') as f:
    f.write(struct.pack('<QQ', 8, 2))     # header: 8 vertices, 2 edges
    f.write(struct.pack('<QQ', 1, 2))     # only 1 edge record written
PYEOF
set +e
OUT=$("$VALIDATOR" "$G" 2>&1); RC=$?
set -e
if [[ $RC -ne 0 ]] && echo "$OUT" | grep -q "failed to read edge record"; then
    pass "truncated edge records → rejected with correct message"
else
    fail "truncated edge records: expected rejection (rc=$RC, out='$OUT')"
fi

# ---- Test 7: boundary value — vertex exactly at MAXV-1 (should succeed) ----
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
