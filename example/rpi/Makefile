CC=gcc
CFLAGS=-I.
DEPS = PCA9685.h
OBJ = PCA9685.o main.o
 
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run_servo: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f *.o 
