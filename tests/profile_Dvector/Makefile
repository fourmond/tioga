# Makefile for profiling

LIB=-lruby1.8
INCLUDE=-I/usr/lib/ruby/1.8/x86_64-linux/ -I.. -I../..
DVECTOR_C=../../split/Dvector.dvector.c

profile_fancy_read: $(DVECTOR_C) profile_fancy_read.c
	gcc -g -O0 -pg $(LIB) $(INCLUDE) -o $@ $^ 
clean:
	-rm profile_fancy_read fancy_read 

fancy_read: $(DVECTOR_C) fancy_read.c
	gcc -g -O2 $(LIB) $(INCLUDE) -o $@ $^ 

prepare_file: prepare_file.c
	gcc -g -O2 -o $@ $^ 
