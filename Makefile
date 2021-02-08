#	Makefile
#	2021-02-07	Markku-Juhani O. Saarinen <mjos@mjos.fi>

#	Demo makefile for S-Box decomposition

#	Cross compile for dosbox on Linux
XCOM	= gostbox.com
NASM	= nasm

#	Regular C target
XBIN	= xtest
CC		= gcc
CFLAGS	= -Wall -Wextra -O2 -g
#CFLAGS  = -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2 -g
CSRC	= $(wildcard *.c)
OBJS	= $(CSRC:.c=.o)
LDLIBS	=

all:	$(XBIN) $(XCOM)

#	dos .com file is made from a single assembler file

%.com:	%.asm
	$(NASM) -f bin $^ -o $@  

#	standard Linux C compile

$(XBIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(XBIN) $(OBJS) $(LDLIBS)

%.o:	%.[cS]
	$(CC) $(CFLAGS) -c $^ -o $@

#	cleanup

clean:
	$(RM) $(XBIN) $(OBJS)

