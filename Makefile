# This is the raw Makefile for thread-runner; you probably
# want config.mk instead.

include config.mk

all: $(MAINOBJS)
	ar rcs libtrunner.a $(MAINOBJS)

install: all
	mkdir -p $(LIBDIR); mkdir -p $(INCLUDEDIR)
	cp libtrunner.a $(LIBDIR)
	cp trunner.h $(INCLUDEDIR)

uninstall:
	rm $(LIBDIR)/libtrunner.a
	rm $(INCLUDEDIR)/trunner.h

clean:
	rm -f $(MAINOBJS)
	rm -f libtrunner.a

docs:
	mkdir -p docs/
	doxygen Doxyfile
