# Makefile
CC = gcc
OBJS = othello.o computer.o
CFLAGS = -g -Wall
PROG = othello
DYM = -DDEBUG

# サフィックスルール
.SUFFIXES: .c .o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $^

.c.o:
	$(CC) $(DYM) $(CFLAGS) -c $<

# ファイルの削除
.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJS)
