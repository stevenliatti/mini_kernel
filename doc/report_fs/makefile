REPORT=report

all: $(REPORT).pdf mostlyclean

$(REPORT).pdf: $(REPORT).tex *.png
	xelatex -shell-escape $(REPORT).tex

mostlyclean:
	rm -rf _minted-* *.log *.aux *.out *.lof *.lol *.toc

clean:
	rm -rf _minted-* $(REPORT).pdf *.log *.aux *.out *.lof *.lol *.toc

rebuild: clean all
