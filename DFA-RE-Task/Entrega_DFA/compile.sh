#!/usr/bin/env bash
# Script para compilar el documento LaTeX de DFAs (Autómatas Finitos Deterministas)

set -e

echo "=== Compilando DFAs.tex ==="
pdflatex -interaction=nonstopmode DFAs.tex
echo "=== Segunda pasada para resolver referencias y figuras ==="
pdflatex -interaction=nonstopmode DFAs.tex

echo "=== Limpiando archivos temporales ==="
rm -f DFAs.aux DFAs.log DFAs.fls DFAs.fdb_latexmk DFAs.synctex.gz

echo "=== Compilación completada con éxito. Archivo generado: DFAs.pdf ==="
