# Makefile for profiling

LIB=-lruby1.8
INCLUDE=-I/usr/lib/ruby/1.8/x86_64-linux/ -I.. -I../..

profile_fancy_read: ../dvector.c profile_fancy_read.c
	gcc -g -O0 -pg $(LIB) $(INCLUDE) -o $@ $^ 
clean:
	-rm profile_fancy_read