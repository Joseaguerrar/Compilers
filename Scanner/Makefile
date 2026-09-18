all:
	$(MAKE) -C src
	ln -sf bin/analyzer scanner

clean:
	$(MAKE) -C src clean
	rm -f scanner

rebuild:
	$(MAKE) -C src rebuild
	ln -sf bin/analyzer scanner

.PHONY: all clean rebuild
