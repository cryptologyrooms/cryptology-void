CC = g++
DEL = @rm -f

ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION = exe
else
	TARGET_EXTENSION = out
endif

LIBS = -lcppunit

CFLAGS = -Wall -Wextra -DUNIT_TEST

%.o: %.cpp
	$(CC) $(INC) $(DEFINES) -c -o $@ $< $(CFLAGS)

all: display.o 
	$(CC) -o display.unittest.$(TARGET_EXTENSION) $^ $(CFLAGS) $(LIBS)
	./display.unittest.$(TARGET_EXTENSION)

clean:
	$(DEL) ./display.unittest.$(TARGET_EXTENSION)

	