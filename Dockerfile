FROM debian:bookworm

# Install basic build tools (C compiler, debugger, etc.)
RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    cmake \
    git \
 && rm -rf /var/lib/apt/lists/*

# Default working directory inside the container
WORKDIR /work
