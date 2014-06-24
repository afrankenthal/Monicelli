PROGRAMS = treeClass readTreeClass addVertices

all: $(PROGRAMS)

treeClass:        treeClass.o          Event.o EventDict.o
	g++ -o    treeClass            treeClass.o            \
                                       Event.o                \
                                       EventDict.o            \
                                       `root-config --libs`
        
treeClass.o:      treeClass.cpp        Event.h
	g++ -c -o treeClass.o          treeClass.cpp          \
                                       `root-config --cflags`

addVertices:      addVertices.o        Event.o EventDict.o
	g++ -o    addVertices          addVertices.o          \
                                       Event.o                \
                                       EventDict.o            \
                                       `root-config --libs`

addVertices.o :   addVertices.cpp      Event.h    
	g++ -c -o addVertices.o        addVertices.cpp        \
                                       `root-config --cflags`

Event.o:          Event.cpp
	g++ -c -o Event.o -I./         Event.cpp              \
                                       `root-config --cflags`
EventDict.o:      EventDict.C
	g++ -c -o EventDict.o          EventDict.C            \
                                       `root-config --cflags`
        
EventDict.C:      Event.h
	rm -f     EventDict.* &&                              \
        rootcint  EventDict.C -c Event.h+ 

readTreeClass:    readTreeClass.o      Event.o EventDict.o
	g++ -o    readTreeClass  -I./  readTreeClass.o        \
                                       Event.o                \
                                       EventDict.o            \
                                       `root-config --libs`

readTreeClass.o:  readTreeClass.cpp
	g++ -c -o readTreeClass.o      readTreeClass.cpp      \
                                       `root-config --cflags`       
        
clean: 
	rm -f *.o $(PROGRAMS) EventDict.C EventDict.h
