# CI-0146 Compiladores e Intérpretes
## Tarea: Expresiones Regulares (Regular Expressions)

**Universidad de Costa Rica (UCR)**  
**Escuela de Ciencias de la Computación e Informática (ECCI)**  
**Profesor:** Dr. Francisco J. Torres-Rojas  
**Fecha de entrega:** 19 de octubre de 2026  

---

### Integrantes del Grupo
- **José Guerra** — Carné: C33510
- **Mario Cordero** — Carné: C22306
- **Daniel Gómez** — Carné: C23310

---

## 1. Descripción del Contenido

Esta carpeta contiene la entrega correspondiente a los ejercicios de **Expresiones Regulares (Regular Expressions)** (ejercicios del **12 al 36** inclusive). Cada ejercicio incluye:
- Descripción del lenguaje sobre el alfabeto dado.
- Expresión regular matemática correspondiente.
- Explicación detallada del funcionamiento de la expresión y casos especiales considerados (cadena vacía $\varepsilon$, repeticiones, precedencia de símbolos).

### Estructura de Archivos

```text
Entrega_Regex/
├── Regex.pdf        # Documento final compilado en formato PDF (listo para lectura)
├── Regex.tex        # Código fuente en LaTeX que genera el documento
├── Makefile         # Archivo de automatización para compilar y limpiar
├── compile.sh       # Script bash para compilar en sistemas Linux/macOS
└── README.md        # Este documento con instrucciones y requerimientos
```

---

## 2. Requerimientos del Sistema

Para compilar el código fuente `Regex.tex` se requiere una distribución estándar de LaTeX:

- **Distribución de LaTeX recomendada:**
  - **Linux (Ubuntu/Debian):** `TeX Live`
    ```bash
    sudo apt update
    sudo apt install texlive-latex-base texlive-latex-extra texlive-fonts-recommended
    ```
  - **macOS:** `MacTeX` (`brew install --cask mactex-no-gui` o MacTeX completo).
  - **Windows:** `MiKTeX` o `TeX Live for Windows`.
  - **Alternativa sin instalación local:** Importar el archivo `Regex.tex` directamente en [Overleaf](https://www.overleaf.com) (compilador por defecto: pdfLaTeX).

- **Paquetes de LaTeX utilizados:**
  - `inputenc` con codificación `[utf8]`
  - `fontenc` con codificación `[T1]`
  - `babel` con opción `[english]`
  - `amsmath`, `amssymb` (entornos matemáticos avanzados)
  - `graphicx`
  - `needspace` (control de saltos de página en bloques de ejercicios)
  - `geometry` (márgenes y tamaño carta)

*(Todos estos paquetes vienen incluidos por defecto en cualquier instalación estándar de TeX Live o MiKTeX).*

---

## 3. Instrucciones de Compilación

### Opción A: Usando `make` (Recomendado si tiene `make` y `pdflatex`)
```bash
# Compilar y generar Regex.pdf
make

# Limpiar archivos auxiliares (.aux, .log, .fls, etc.)
make clean
```

### Opción B: Usando el script `compile.sh`
```bash
# Dar permisos de ejecución (si no los tiene)
chmod +x compile.sh

# Ejecutar el script de compilación
./compile.sh
```

### Opción C: Usando la terminal directamente con `pdflatex`
Se recomienda ejecutar el comando **dos veces** para que se resuelvan correctamente las referencias internas y contadores:
```bash
pdflatex -interaction=nonstopmode Regex.tex
pdflatex -interaction=nonstopmode Regex.tex
```

### Opción D: Usando `latexmk`
Si tiene instalada la utilidad `latexmk`:
```bash
latexmk -pdf Regex.tex
```
Para limpiar archivos auxiliares con `latexmk`:
```bash
latexmk -c Regex.tex
```

### Opción E: Desde un editor gráfico de LaTeX
- **VS Code:** Abrir `Regex.tex` con la extensión *LaTeX Workshop* instalada y presionar `Ctrl + Alt + B` (o `Cmd + Option + B` en Mac).
- **TeXworks / TeXstudio / TeXmaker:** Abrir `Regex.tex`, seleccionar motor **pdfLaTeX** en el menú desplegable y presionar el botón de **Compilar / Build**.
- **Overleaf:** Crear un nuevo proyecto vacío, subir `Regex.tex` y presionar **Recompile**.

---

## 4. Limpieza de Archivos Temporales

La compilación en LaTeX produce archivos intermedios (`.aux`, `.log`, `.fls`, `.fdb_latexmk`, `.synctex.gz`). Si desea eliminarlos manualmente:

```bash
rm -f *.aux *.log *.fls *.fdb_latexmk *.synctex.gz
```
O ejecutando:
```bash
make clean
```
