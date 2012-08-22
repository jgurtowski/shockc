CC = gcc
CFLAGS = -O2 -Wall
LIB=-lcurl
LIB_OBJECTS=shock_config.o shock_connection.o shock_error.o json.o
BIN_OBJECTS=$(LIB_OBJECTS) main.o


%.o : %.c
	$(CC) $(CFLAGS) -c $<

all: shockc libshockc

shockc: $(BIN_OBJECTS)
	$(CC) $(LIB) $(CFLAGS) $(BIN_OBJECTS) -o shockc

libshockc: $(LIB_OBJECTS)
	$(CC) $(LIB) $(CFLAGS) -shared $(LIB_OBJECTS) -o libshockc.so

clean:
	rm -f *.o shockc libshockc.so