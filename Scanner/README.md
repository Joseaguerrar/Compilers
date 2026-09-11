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

# Proyect 1: Lexical Analysis

## Requirements

1. Install `Flex`

```
sudo apt-get update
sudo apt-get install flex
```

## Compilation

1. Compile `.l` or `.lex` extension program

```
flex *.l
```

2. Compile created `lex.yy.c` with gcc.

```
gcc lex.yy.c
```

3. Execute

```
./a.out
```