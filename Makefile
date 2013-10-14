
CC = gcc
CFLAGS = `pkg-config opencv --cflags --libs`
LDFLAGS = 
DEPS = blob.h rs232.h
SRCS = $(DEPS:.h=.c) main.c
OBJS = $(SRCS:.c=.o)
EXEC = pick


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
    

clean:
	rm -f *.o
