CC = gcc
FLAGS = -Wall -c
# -errors -Wpedantic -pedantic
LDFLAGS =
EXT = .c

EXECUTABLE1 = compiler

SOURCEDIR = ./
# BUILDDIR = ./../../build/$(EXECUTABLE1)/
# BINDIR = ./../../bin

SOURCES = $(wildcard $(SOURCEDIR)/*$(EXT))
OBJECTS = $(patsubst $(SOURCEDIR)/%$(EXT),$(BUILDDIR)/%.o,$(SOURCES))

all:   dir $(BINDIR)/$(EXECUTABLE1)

dir:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)

$(BINDIR)/$(EXECUTABLE1): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%$(EXT)
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -f $(BUILDDIR)/*.o $(BINDIR)/$(EXECUTABLE1)
