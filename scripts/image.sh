#!/usr/bin/env bash
# Builds the course's Docker image (Zephyr + SDK + code-server) from course/Dockerfile.espressif
set -euo pipefail
source "$(dirname "$0")/lib.sh"
[ -f "$ROOT/course/Dockerfile.espressif" ] || die "course/ missing. Run: make bootstrap"
ARCH=$(uname -m); [ "$ARCH" = arm64 ] && TA=arm64 || TA=amd64
EXTRA=""; [ -n "${PLATFORM:-}" ] && { EXTRA="--platform $PLATFORM"; TA="${PLATFORM#linux/}"; }
info "Building $IMAGE for $TA (first build: ~20–40 min, ~10 GB)"
docker build $EXTRA --build-arg TARGETARCH="$TA" -t "$IMAGE" -f "$ROOT/course/Dockerfile.espressif" "$ROOT/course"
info "Done. Next: make up"
