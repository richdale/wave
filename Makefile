
CC = gcc
CFLAGS = `pkg-config opencv --cflags --libs`
LDFLAGS = 
DEPS = 
SRCS = $(DEPS:.h=.c) main.c
OBJS = $(SRCS:.c=.o)
EXEC = rec


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
    

clean:
	rm -f *.o
