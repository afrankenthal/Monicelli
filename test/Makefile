EXES = master server1 server2 ipcsMonitor makeComplexFile exploreComplexFile

all: $(EXES)

master: obj/master.o ../obj/MessageBus.o
	@echo "Building master"
	g++ -o master obj/master.o ../obj/MessageBus.o -L$(BOOSTLIB) -lboost_regex -lboost_filesystem
        
obj/master.o: master.cc ../include/MessageBus.h ../include/msg.h
	@echo "Compiling master.cc"
	g++ -c -I../include -o obj/master.o master.cc

server1: obj/server1.o ../obj/MessageBus.o
	@echo "Building server1"
	g++ -o server1 obj/server1.o ../obj/MessageBus.o -L$(BOOSTLIB) -lboost_regex -lboost_filesystem
        
obj/server1.o: server1.cc ../include/MessageBus.h ../include/msg.h
	@echo "Compiling server1.cc"
	g++ -c -I../include -o obj/server1.o server1.cc

server2: obj/server2.o ../obj/MessageBus.o
	@echo "Building server2"
	g++ -o server2 obj/server2.o ../obj/MessageBus.o -L$(BOOSTLIB) -lboost_regex -lboost_filesystem
        
obj/server2.o: server2.cc ../include/MessageBus.h ../include/msg.h
	@echo "Compiling server2.cc"
	g++ -c -I../include -o obj/server2.o server2.cc

../obj/MessageBus.o: ../src/MessageBus.cpp ../include/MessageBus.h ../include/msg.h
	@echo "Compiling src/MessageBus.cpp"
	g++ -c -I../include -o ../obj/MessageBus.o ../src/MessageBus.cpp
        
ipcsMonitor: obj/ipcsMonitor.o
	@echo "Building ipcsMonitor"
	g++ -o ipcsMonitor obj/ipcsMonitor.o -L$(BOOSTLIB) -lboost_regex -lboost_filesystem

obj/ipcsMonitor.o: ipcsMonitor.cpp
	@echo "Compiling ipcsMonitor"
	g++ -c -o obj/ipcsMonitor.o ipcsMonitor.cpp -I../include

./trees/Event.o: ./trees/Event.cpp
	@echo "Compiling Event"
	g++ -c -o ./trees/Event.o             \
                  -I./ ./trees/Event.cpp      \
                  `root-config --cflags`
                  
./trees/EventDict.o: ./trees/EventDict.C
	@echo "Compiling EventDict"
	g++ -c -o ./trees/EventDict.o         \
                  ./trees/EventDict.C         \
                  -I./                        \
                  `root-config --cflags`
        
./trees/EventDict.C: ./trees/Event.h
	@echo "rootcint EventDict"
	rm -f ./trees/EventDict.* &&          \
	rootcint ./trees/EventDict.C -c -f    \
                 ./trees/Event.h+             

makeComplexFile: obj/makeComplexFile.o        \
                 ./trees/Event.o              \
                 ./trees/EventDict.o
	@echo "Building makeComplexFile"
	g++ -o makeComplexFile                \
               obj/makeComplexFile.o          \
               ./trees/Event.o                \
               ./trees/EventDict.o            \
               `root-config --libs`
	
obj/makeComplexFile.o: makeComplexFile.cpp
	@echo "Compiling makeComplexFile"
	g++ -c -o obj/makeComplexFile.o       \
                  -I./trees                   \
        	  makeComplexFile.cpp         \
                  `root-config --cflags`        

exploreComplexFile: obj/exploreComplexFile.o  \
                 ./trees/Event.o              \
                 ./trees/EventDict.o
	@echo "Building exploreComplexFile"
	g++ -o exploreComplexFile             \
               obj/exploreComplexFile.o       \
               ./trees/Event.o                \
               ./trees/EventDict.o            \
               `root-config --libs`
	
obj/exploreComplexFile.o: exploreComplexFile.cpp
	@echo "Compiling exploreComplexFile"
	g++ -c -o obj/exploreComplexFile.o    \
                  -I./trees                   \
        	  exploreComplexFile.cpp      \
                  `root-config --cflags`        

clean:
	rm -f obj/*.o $(EXES) 
