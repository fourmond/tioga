# Makefile for Tioga Tutorial

# compiler information -- change as you wish
C_COMPILER = gcc
C_FLAGS = -g -c -ansi -W -Wall

BUILDER = builder
BUILDER_OBJS = builder.o

$(BUILDER) : $(BUILDER_OBJS) 
	$(C_COMPILER) -o $(BUILDER) $(BUILDER_OBJS) -lm

%.o: %.c
	$(C_COMPILER) -I$(EZVIEW_INCDIR) $(C_FLAGS) $<

clean : 
	-@rm -f *.o
