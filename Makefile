# First target is default target, if you just type:  make

FILE=cache.c

default: run

run: cache
	./cache

gdb: cache
	gdb --args cache

cache: ${FILE}
	gcc -g -lm -O0 -o cache ${FILE}

emacs: ${FILE}
	emacs ${FILE}
vi: ${FILE}
	vi ${FILE}

clean:
	rm -f cache a.out *~
# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
