# Makefile para compilar documento de Expresiones Regulares (LaTeX)

DOC = Regex

.PHONY: all clean distclean view

all: $(DOC).pdf

$(DOC).pdf: $(DOC).tex
	pdflatex -interaction=nonstopmode $(DOC).tex
	pdflatex -interaction=nonstopmode $(DOC).tex

clean:
	rm -f $(DOC).aux $(DOC).log $(DOC).fls $(DOC).fdb_latexmk $(DOC).synctex.gz

distclean: clean
	rm -f $(DOC).pdf

view: $(DOC).pdf
	@which xdg-open > /dev/null && xdg-open $(DOC).pdf || open $(DOC).pdf
