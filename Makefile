EXECUTABLE=mandel
SOURCES=main.cpp formulas.cpp sys.cpp

CC=g++
CFLAGS=-Wall
LIBS=-lalleg
LDFLAGS=
BUILDDIR=build
OBJECTS=$(SOURCES:%.cpp=$(BUILDDIR)/%.o)

.PHONY: clean directories

all: directories $(SOURCES) $(EXECUTABLE)

$(BUILDDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -fr $(BUILDDIR) *~ $(EXECUTABLE)

directories: $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $@
