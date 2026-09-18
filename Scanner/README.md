# Proyecto 1: Analizador Léxico

## Instrucciones

## Ejecución

1. Programa se invocará desde línea de comando de una consola.
    * Argumentos:
        * Nombre del archivo fuente
        * Opciones al estilo UNIX/Linux

2. Acepta cualquier tipo de extensión.

## Preproceso

1. Directivas
    * #include
        * Permite incluir un archivo de texto como parte del fuente (no hacer suposición respecto a extensión o contenido)
        * Archivo siempre reside en el directorio actual
        * Puede contener nuevo directivas de preproceso.

    * #define
        * Asocia un nombre (mismas reglas de los nombres de variables) con una hilera de texto formado por el resto de carácteres de la línea del archivo.
        * Algunos deben ser expandidos. No se manejarán macros con parámetros (**TRABAJO EXTRA OPCIONAL 1**).

2. Eliminar los comentarios contenidos en el archivo de entrada (manejar las dos formas de comentarios de C).

## *Scanner*

1. Utilizar la herramienta *flex*. Es permitido utilizarse en el preprocesador.

2. *Scanner* es para el lenguaje C.

3. Debe presentar una función semejante al `Get_Token()` estudiado en clase. Al ser invocado, regresará el siguiente *token* del fuente procesado en alguna estructura de datos apropiada que tenga cosas como: código de *token*, puntero al lexema, valor numérico del lexema, etc. Estructurar programa para que `Get_Token()` sea independiente del resto del código.

4. Entrada de esta parte del programa es el archivo de texto temproal generado por el preprocesador. Presumimos que es un programa escrito en C, pero podría ser cualquier archivo, ya sea texto o binario.

## Salida

1. Debe producir una presentación `Beamer`.

    * Debe generar un programa fuente de `Latex`.
    * Se ejecutará el comando `pdflatex` que procese dicho archivo y genere un PDF.
    * Se despliega de inmediatio (usando el comando `evince`) en modo presentación.

2. Presentación `Beamer` debe incluir lo siguiente:
    * Slide de portada:
        * Identificador de Grupo de Trabajo
        * Semestre
        * Proyecto

    * Varios slides con una explicación general del proceso de *scanning* y de la herramiente `flex`.

    * Multiples slides con el programa fuente que le entró en la fase de *scanning* (después del preproceso), pero con una distinción de cada lexema.
        * Distintos tipos de letra
        * Pesos
        * Inclinaciones
        * Combinaciones de colores (juegos de *foreground* y *background*) para cada categoría léxica.
        * Errores léxicos deben ser reportados de la misma manera.
        * Poner una cantidad razonable de líneas del fuente original en cada *slide*.

    * Histograma de las cantidades de cada tipo de *token* encontrados en el fuente.
        * Preparar un archivo de datos e invocar al comando `gnuplot` desde el programa o usar el package `pgfplots` de `Latex`.

## Requisitos del Sistema y Dependencias

Para compilar y ejecutar el proyecto (incluyendo la generación y visualización de la presentación Beamer), se requieren:
- **Compilador C / Herramientas de construcción:** `gcc`, `make`
- **Generador léxico:** `flex`
- **Motor LaTeX y paquetes Beamer/Pgfplots:** `pdflatex`, `beamer`, `pgfplots`, `tikz`, `listings`
- **Visor de PDF en modo presentación:** `evince`

### Instalación de dependencias por distribución

#### Arch Linux / EndeavourOS / Manjaro
```bash
sudo pacman -S flex gcc make texlive-basic texlive-latex texlive-latexrecommended texlive-latexextra texlive-pictures evince
```

#### Ubuntu / Debian / Linux Mint
```bash
sudo apt update
sudo apt install -y flex gcc make texlive-latex-base texlive-latex-recommended texlive-latex-extra texlive-pictures evince
```

#### Fedora
```bash
sudo dnf install -y flex gcc make texlive-scheme-basic texlive-beamer texlive-pgf texlive-pgfplots evince
```

---

## Compilación

Desde la carpeta raíz del proyecto:

```bash
make
```

Esto generará automáticamente el ejecutable y el acceso directo `./scanner` en la raíz.

Otras opciones de compilación:
- **Limpiar binarios y temporales:** `make clean`
- **Recompilar todo desde cero:** `make rebuild`

---

## Ejecución

Para analizar un archivo fuente y abrir automáticamente la presentación Beamer:

```bash
./scanner <archivo_fuente.c>
```

### Ejemplos con casos de prueba incluidos:

```bash
# Programa C completo (demostración principal)
./scanner tests/18_programa_completo.c

# Probar todas las palabras reservadas
./scanner tests/10_todas_keywords.c

# Probar constantes numéricas (hex, octal, float, chars)
./scanner tests/11_constantes_numericas.c

# Probar macros recursivas y directivas de preproceso
./scanner tests/05_define_recursivo.c

# Probar manejo de errores léxicos
./scanner tests/09_caracteres_ilegales.c
```