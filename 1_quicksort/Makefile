CC = /opt/opencilk/bin/clang++
CFLAGS = -fopencilk -ltbb -O3 -Werror -Wall -Wextra -pedantic

SOURCES = main.cpp
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

clean:
	rm -f $(EXECUTABLE)

run: $(EXECUTABLE)
	CILK_NWORKERS=4 ./$(EXECUTABLE)
