SUBDIRS = src

all: debug

debug:
	for dir in $(SUBDIRS); do make -C $$dir debug; done

optimized:
	for dir in $(SUBDIRS); do make -C $$dir optimized; done

clean:
	for dir in $(SUBDIRS); do make -C $$dir clean; done
	rm -f *~ *.o_dbg *.o_opt *.bak output.txt *.cnf
