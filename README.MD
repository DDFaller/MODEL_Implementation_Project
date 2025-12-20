<!--
MACHADO CARNEIRO FALLER Daniel (21400117)
LY Laura (21500152)
CRIVELLARO Federico (21502450)
-->

## Workflow (Docker)
Recommended for the most consistent environment across machines.

### 1) Build the Docker image
From the project root:
```bash
docker build -t model-c .
```

### 2) Start the container (Windows)
Run:
```bat
run-docker.bat
```
This mounts the project at `/work` inside the container.

### 3) Install MPFR/GMP inside the container
```bash
./install_mpfr.sh
```

### 4) Build or run tests
```bash
make
```
Or run unit tests:
```bash
make test
```

## Workflow (Native, no Docker)
If you prefer to run locally, install the dependencies and build with `make`.

### Windows (native)
1) Install a GCC toolchain and `make` (MSYS2 or WSL).  
2) Install MPFR and GMP development packages for your toolchain.  
3) From the project root:
```bash
make
```
Or run unit tests:
```bash
make test
```

### Linux (native)
1) Install build tools and MPFR/GMP:
```bash
sudo apt-get update
sudo apt-get install -y build-essential libmpfr-dev libgmp-dev
```
2) From the project root:
```bash
make
```
Or run unit tests:
```bash
make test
```

