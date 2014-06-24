BASEDIR    = .
CDIR       = $(BASEDIR)/src
ODIR       = $(BASEDIR)/obj
LDIR       = $(BASEDIR)/lib
BINDIR     = $(BASEDIR)/bin
IDIR       = $(BASEDIR)/interface

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)
TYPEINFO   = /usr/local/root/cint/include

CC         = g++
CFLAGS     = -Wall -I$(IDIR)


# Add header files from interface directory here
_DEPS      = CommonVariables.h   \
             Cuts.h              \
	     DataClass.h         \
	     HistogramWindow.h   \
	     HManager.h          \
	     Plane.h             \
	     RectangularWindow.h \
	     Utils.h             \
	     Window.h 
DEPS       = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Compile .cpp implementation files into object files
_OBJ       = Analysis.o 	 \
             Cuts.o		 \
	     DataClass.o	 \
	     HistogramWindow.o   \
	     HManager.o 	 \
	     Plane.o		 \
	     RectangularWindow.o \
	     Window.o 
OBJ        = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(CDIR)/%.cpp $(DEPS)
	@echo ' '
	@echo '---------------------- o ----------------------------'
	@echo 'Building $@'
	$(CC) -c -o $@ $< $(CFLAGS) -I. $(ROOTCFLAGS) 
	@echo '-----------------------------------------------------'

$(BINDIR)/TBAnalysis: $(OBJ)
	@echo ' '
	@echo '---------------------- e ----------------------------'
	@echo 'Building $@'
	$(CC)    -o $@ $^ $(CFLAGS) -I. $(ROOTCFLAGS) $(ROOTLIBS)
	@echo '-----------------------------------------------------'

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BINDIR)/* 
