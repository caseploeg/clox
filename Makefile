CC = gcc

TARGET = clox
SRCS = chunk.c compiler.c debug.c main.c memory.c object.c scanner.c table.c value.c vm.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

