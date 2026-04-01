#!/usr/bin/env bash
set -e

IMAGE="switchu-builder"
OUTPUT="build/cross/none/release"

# Build the Docker image if it doesn't exist yet
if ! docker image inspect "$IMAGE" &>/dev/null; then
    echo "Building Docker image '$IMAGE'..."
    docker build -t "$IMAGE" .
fi

# Run the build, mounting the project into the container
docker run --rm \
    -v "$(pwd)":/project \
    "$IMAGE"

echo ""
echo "Build complete. Output files:"
ls -lh "$OUTPUT"/*.nsp 2>/dev/null || echo "No .nsp files found in $OUTPUT"
