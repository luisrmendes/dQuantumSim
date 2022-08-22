PROG := dqsim
IDIR := include
SDIR := src
ODIR := obj

CC := mpicxx
CPPFLAGS := -I$(IDIR) -std=c++17 -lstdc++fs -Wall \
-Wextra -pedantic -m64 -pipe -O3
LDFLAGS := 

DEPS := $(wildcard $(IDIR)/*.h)

_SRC := $(wildcard $(SDIR)/*.cpp)
_OBJ := $(_SRC:.cpp=.o)
OBJ := $(subst $(SDIR),$(ODIR),$(_OBJ))

$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDFLAGS) $(TARGETS)

$(OBJ): $(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(TARGETS)

.PHONY: clean

clean:
	rm -rf $(ODIR)
	rm -f $(PROG)

$(info $(shell mkdir -p $(ODIR)))