CC = gcc
FLAGS = -Wall -c
# -errors -Wpedantic -pedantic
LDFLAGS =
EXT = .c

SOURCEDIR = ./

SOURCES = $(wildcard $(SOURCEDIR)/*$(EXT))
OBJECTS = $(patsubst $(SOURCEDIR)/%$(EXT),$(BUILDDIR)/%.o,$(SOURCES))

all:   dir $(BINDIR)/$(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)

$(BINDIR)/$(EXECUTABLE): $(OBJECTS)
	#$(CC) $(LDFLAGS) $^ -o $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%$(EXT)
	$(CC) $(FLAGS) $< -o $@

clean:
	rm -f $(BUILDDIR)/*.o $(BINDIR)/$(EXECUTABLE)
