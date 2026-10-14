#!/usr/bin/env bash
# Starts the dev container.
#   /workspace  <- ./workspace          (your code, read-write)
#   /course     <- ./course/workspace   (reference code, READ-ONLY)
# Port 3333 is NOT published on purpose: OpenOCD runs on the Mac and the
# container reaches it via host.docker.internal:3333 (publishing it would block OpenOCD).
set -euo pipefail
source "$(dirname "$0")/lib.sh"
if docker ps --format '{{.Names}}' | grep -qx "$CONTAINER"; then info "$CONTAINER already running → http://localhost:8800"; exit 0; fi
docker image inspect "$IMAGE" >/dev/null 2>&1 || die "Image $IMAGE not found. Run: make image"
mkdir -p "$ROOT/workspace/build"
docker run -d --rm --name "$CONTAINER" \
  -p 2222:22 -p 8800:8800 \
  -v "$ROOT/workspace:/workspace" \
  -v "$ROOT/course/workspace:/course:ro" \
  -w /workspace "$IMAGE" >/dev/null
info "Started $CONTAINER → browser VS Code: http://localhost:8800"
info "Or VS Code Dev Containers: 'Attach to Running Container' → $CONTAINER → open /zephyr.code-workspace"
