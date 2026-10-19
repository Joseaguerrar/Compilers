#!/usr/bin/env bash
# Script para compilar el documento LaTeX de Expresiones Regulares

set -e

echo "=== Compilando Regex.tex ==="
pdflatex -interaction=nonstopmode Regex.tex
echo "=== Segunda pasada para resolver referencias ==="
pdflatex -interaction=nonstopmode Regex.tex

echo "=== Limpiando archivos temporales ==="
rm -f Regex.aux Regex.log Regex.fls Regex.fdb_latexmk Regex.synctex.gz

echo "=== Compilación completada con éxito. Archivo generado: Regex.pdf ==="
