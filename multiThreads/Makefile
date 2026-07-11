

CC = gcc
CFLAGS = -Wall -g -std=c11
LDFLAGS = -pthread

OBJS = pthread_wrapper.c
TARGET = main

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o : %.c
	(CC) $(FLAGS) -c %< -o %@

clean:
	rm -rf *.o $(TARGET)

run:
	./$(TARGET)