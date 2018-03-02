GOALS = ./lib ./nyasm ./emulator
GLOBALFLAGS =

EXT   = .c

SRC_DIR = ./src
BLD_DIR = ./build
BIN_DIR = ./bin

REL_PTH = ../../

EXES    = $(patsubst %, $(BIN_DIR)/%, $(GOALS))

all: $(GOALS)

clean:
	rm    -f $(EXES)
	rm   -rf $(BLD_DIR)
	rmdir --ignore-fail-on-non-empty $(BIN_DIR)

$(GOALS): % : $(SRC_DIR)/%
	cd $< && make                        \
	"GLOBALFLAGS"=$(GLOBALFLAGS)         \
	"REL_PTH"=$(REL_PTH)                 \
	"BUILDDIR"=$(REL_PTH)/$(BLD_DIR)/$@  \
	"BINDIR"=$(REL_PTH)/$(BIN_DIR)/      \
	"LIBDIR"=lib                         \
	"EXECUTABLE"=$@
