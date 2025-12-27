# Build stage
FROM ubuntu:22.04 as builder

WORKDIR /build

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
COPY . /build/

# Build the application
RUN mkdir build_dir && cd build_dir && \
    cmake .. && \
    cmake --build . -- -j$(nproc)

# Runtime stage
FROM ubuntu:22.04

WORKDIR /app

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binary from builder
COPY --from=builder /build/build_dir/bin/streaming_app /app/streaming_app

# Expose ports
EXPOSE 5000 5001

# Run the application
CMD ["./streaming_app"]
