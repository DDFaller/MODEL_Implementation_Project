#!/bin/bash

apt-get update

echo "Installing bibliotecas MPFR e GMP..."
apt-get install -y libmpfr-dev libgmp-dev

echo "Installed"