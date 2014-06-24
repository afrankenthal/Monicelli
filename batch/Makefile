EXELIST      = risi zampa


LIBS         = -L$(ROOTSYS)/lib -lASImageGui   -lASImage           -lCintex       -lCint         -lcomplexDict     -lCore       \
                                -ldequeDict    -lEG                -lEve          -lFitPanel     -lFoam            -lFTGL       \
                                -lFumili       -lGed               -lGenVector    -lGeomBuilder  -lGeomPainter     -lGeom       \
                                -lGFAL         -lGLEW              -lGpad         -lGQt          -lGraf3d          -lGraf       \
                                -lGuiBld       -lGuiHtml           -lGui          -lGviz3d       -lGX11            -lGX11TTF    \
                                -lHbook        -lHistPainter       -lHist         -lHtml         -llistDict        -lmap2Dict   \
                                -lmapDict      -lMathCore          -lMatrix       -lMemStat      -lminicern        -lMinuit     \
                                -lMLP          -lmultimap2Dict     -lmultimapDict -lmultisetDict -lNet             -lNetx       \
                                -lNew          -lPhysics           -lPostscript   -lPyROOT       -lQtRoot          -lQuadp      \
                                -lRecorder     -lReflexDict        -lReflex       -lRGL          -lRint            -lRIO        \
                                -lRLDAP        -lRooFitCore        -lRooFit       -lRooStats     -lRootAuth        -lsetDict    \
                                -lSmatrix      -lSpectrumPainter   -lSpectrum     -lSPlot        -lSQLIO           -lSrvAuth    \
                                -lTable        -lThread            -lTMVA         -lTreePlayer   -lTree            -lTreeViewer \
                                -lvalarrayDict -lvectorDict        -lVMC          -lX3d          -lXMLIO           -lXMLParser  \
                                -lXrdBwm       -lXrdClient         -lXrdCrypto    -lXrdCryptossl -lXrdPosixPreload -lXrdPosix   \
                                -lXrdProxy     -lXrdSecgsiGMAPLDAP -lXrdSecgsi    -lXrdSecpwd    -lXrdSec          -lXrdSecssl  \
                                -lXrdSecsss    -lXrdSecunix        -lXrdSut                                                     \
                                -pthread       -lm                 -ldl           -rdynamic                                     \
               -L$(BOOSTLIB)    -lboost_regex  -lboost_filesystem

all: $(EXELIST)

risi: obj/risi.o obj/alphanum.o          \
                 obj/fileEater.o         \
                 obj/Event.o             \
                 obj/MessageBus.o        \
                 obj/EventDict.o
	g++ -o risi                      \
            $(LIBS)                      \
            obj/alphanum.o               \
            obj/Event.o                  \
            obj/EventDict.o              \
            obj/fileEater.o              \
            obj/MessageBus.o             \
            obj/risi.o 

obj/risi.o: risi.cpp
	g++ -c -o obj/risi.o             \
            -I../include                 \
            -Iinc                        \
            -I$(ROOTSYS)/include         \
            -I$(BOOSTINC)/include/boost/ \
            risi.cpp

zampa: obj/zampa.o obj/alphanum.o        \
                   obj/fileEater.o       \
                   obj/Event.o           \
                   obj/MessageBus.o      \
                   obj/EventDict.o
	g++ -o zampa                     \
	    $(LIBS)                      \
            obj/alphanum.o               \
            obj/Event.o                  \
            obj/EventDict.o              \
            obj/fileEater.o              \
            obj/MessageBus.o             \
            obj/zampa.o

obj/zampa.o: zampa.cpp
	g++ -c -o obj/zampa.o            \
	    -I../include                 \
	    -Iinc                        \
	    -I$(ROOTSYS)/include         \
	    -I$(BOOSTINC)/include/boost/ \
	    zampa.cpp

obj/fileEater.o: ../src/fileEater.cpp    \
                 ../include/fileEater.h
	g++ -c -o obj/fileEater.o        \
	    -I../include                 \
            -I$(ROOTSYS)/include         \
            -I$(BOOSTINC)/include/boost/ \
            ../src/fileEater.cpp
            
obj/Event.o: ../src/Event.cpp ../include/Event.h
	g++ -c -o obj/Event.o            \
	    -I../include                 \
            -I$(ROOTSYS)/include         \
            -I$(BOOSTINC)/include/boost/ \
            ../src/Event.cpp
	
obj/MessageBus.o: ../src/MessageBus.cpp  \
                  ../include/MessageBus.h
	g++ -c -o obj/MessageBus.o       \
	    -I../include                 \
            -I$(ROOTSYS)/include         \
            -I$(BOOSTINC)/include/boost/ \
            ../src/MessageBus.cpp

obj/EventDict.o: tmp/EventDict.C         \
                 ../include/Event.h
	g++ -c -o obj/EventDict.o        \
	    -I../include                 \
            -I$(ROOTSYS)/include         \
            tmp/EventDict.C

tmp/EventDict.C: ../include/Event.h
	rootcint -f tmp/EventDict.C -c ../include/Event.h+

obj/alphanum.o : src/alphanum.cc         \
                 inc/alphanum.h
	 g++ -c -o $@                    \
	     -I./inc                     \
	           $<  	                 
clean: 
	rm -f tmp/*.C obj/*.o $(EXELIST)
