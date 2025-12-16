#!/bin/bash

echo "Atualizando repositórios..."
apt-get update

echo "Instalando bibliotecas MPFR e GMP..."
apt-get install -y libmpfr-dev libgmp-dev

echo "Verificando instalação..."
pkg-config --exists mpfr && echo "MPFR instalado com sucesso" || echo "Erro na instalação do MPFR"
pkg-config --exists gmp && echo "GMP instalado com sucesso" || echo "Erro na instalação do GMP"

echo "Instalação concluída!"