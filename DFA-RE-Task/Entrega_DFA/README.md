# CI-0146 Compiladores e Intérpretes
## Tarea: Autómatas Finitos Deterministas (Deterministic Finite Automata - DFA)

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

Esta carpeta contiene la entrega correspondiente a los ejercicios de **Autómatas Finitos Deterministas (DFA)** (ejercicios del **12 al 36** inclusive). Cada ejercicio incluye:
- Descripción formal del lenguaje sobre el alfabeto dado.
- Diagrama de estados del autómata generado de forma nativa y vectorial mediante **TikZ**, especificando estado inicial, estados de aceptación (doble círculo), transiciones etiquetadas y estados de trampa (trap/sink states) cuando corresponde.
- Explicación de la lógica del autómata y justificación de cada estado.

### Estructura de Archivos

```text
Entrega_DFA/
├── DFAs.pdf         # Documento final compilado en formato PDF (listo para lectura)
├── DFAs.tex         # Código fuente en LaTeX que genera el documento (incluye código TikZ)
├── Makefile         # Archivo de automatización para compilar y limpiar
├── compile.sh       # Script bash para compilar en sistemas Linux/macOS
└── README.md        # Este documento con instrucciones y requerimientos
```

---

## 2. Requerimientos del Sistema

Para compilar el código fuente `DFAs.tex` se requiere una distribución de LaTeX con soporte para gráficos vectoriales **TikZ**:

- **Distribución de LaTeX recomendada:**
  - **Linux (Ubuntu/Debian):** `TeX Live`
    ```bash
    sudo apt update
    sudo apt install texlive-latex-base texlive-latex-extra texlive-pictures texlive-fonts-recommended
    ```
  - **macOS:** `MacTeX` (`brew install --cask mactex-no-gui` o MacTeX completo).
  - **Windows:** `MiKTeX` o `TeX Live for Windows`.
  - **Alternativa sin instalación local:** Importar `DFAs.tex` directamente en [Overleaf](https://www.overleaf.com) (compilador por defecto: pdfLaTeX).

- **Paquetes y Librerías de LaTeX utilizados:**
  - `inputenc` con codificación `[utf8]`
  - `fontenc` con codificación `[T1]`
  - `babel` con opción `[english]`
  - `amsmath`, `amssymb`
  - `graphicx`
  - `needspace` (control de flujo y saltos entre figuras)
  - `geometry` (márgenes y tamaño carta)
  - `tikz` con las siguientes librerías:
    - `automata` (para nodos `state`, estados iniciales y de aceptación)
    - `positioning` (posicionamiento relativo de estados)
    - `arrows.meta` (flechas estilizadas `Stealth`)

*(No se requiere ninguna imagen o archivo binario externo; todos los autómatas están codificados vectorialmente en TikZ).*

---

## 3. Instrucciones de Compilación

### Opción A: Usando `make` (Recomendado si tiene `make` y `pdflatex`)
```bash
# Compilar y generar DFAs.pdf
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
Se recomienda ejecutar el comando **dos veces** para que se resuelvan adecuadamente los contadores de figuras (`Figure 1`, `Figure 2`, ...) y saltos de página:
```bash
pdflatex -interaction=nonstopmode DFAs.tex
pdflatex -interaction=nonstopmode DFAs.tex
```

### Opción D: Usando `latexmk`
Si tiene instalada la utilidad `latexmk`:
```bash
latexmk -pdf DFAs.tex
```
Para limpiar archivos auxiliares con `latexmk`:
```bash
latexmk -c DFAs.tex
```

### Opción E: Desde un editor gráfico de LaTeX
- **VS Code:** Abrir `DFAs.tex` con la extensión *LaTeX Workshop* instalada y presionar `Ctrl + Alt + B` (o `Cmd + Option + B` en Mac).
- **TeXworks / TeXstudio / TeXmaker:** Abrir `DFAs.tex`, seleccionar motor **pdfLaTeX** en el menú desplegable y presionar el botón de **Compilar / Build**.
- **Overleaf:** Crear un nuevo proyecto vacío, subir `DFAs.tex` y presionar **Recompile**.

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
