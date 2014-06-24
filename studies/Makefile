PROGRAMS = thisisthebeginning

all: $(PROGRAMS)

thisisthebeginning: obj/thisisthebeginning.o                       \
                    ${MonicelliDir}/Event.o                        \
                    ${MonicelliDir}/Geometry.o                     \
                    ${MonicelliDir}/Detector.o                     \
                    ${MonicelliDir}/ROC.o                          \
                    ${MonicelliDir}/tmp/EventDict.o
	@echo ' '
	@echo '---------------- Creating executable thisisthebeginning ----------'
	g++ -o thisisthebeginning                                  \
               obj/thisisthebeginning.o                            \
               ${MonicelliDir}/Event.o                             \
               ${MonicelliDir}/Geometry.o                          \
               ${MonicelliDir}/Detector.o                          \
               ${MonicelliDir}/ROC.o                               \
               ${MonicelliDir}/tmp/EventDict.o                     \
               `root-config --libs`
        
obj/thisisthebeginning.o: src/thisisthebeginning.cpp               \
                          ${MonicelliDir}/include/Event.h          \
                          ${MonicelliDir}/include/Geometry.h
	@echo ' '
	@echo '---------------- Compiling thisisthebeginning ----------'
	g++ -c                                                     \
            -o obj/thisisthebeginning.o                            \
            -I${MonicelliDir}/include                              \
            src/thisisthebeginning.cpp                             \
            `root-config --cflags`

${MonicelliDir}/Event.o: ${MonicelliDir}/src/Event.cpp             \
	                 ${MonicelliDir}/include/MessageTools.h    \
	                 ${MonicelliDir}/include/ANSIColors.h
	@echo ' '
	@echo '---------------- Compiling Event ----------'
	g++ -c                                                     \
            -o ${MonicelliDir}/Event.o                             \
            -I${MonicelliDir}/include                              \
            ${MonicelliDir}/src/Event.cpp                          \
            `root-config --cflags`
                                       
${MonicelliDir}/Geometry.o: ${MonicelliDir}/include/Detector.h     \
                            ${MonicelliDir}/include/MessageTools.h \
                            ${MonicelliDir}/include/ANSIColors.h
	@echo ' '
	@echo '---------------- Compiling Geometry ----------'
	g++ -c                                                     \
            -o ${MonicelliDir}/Geometry.o                          \
            -I${MonicelliDir}/include                              \
            ${MonicelliDir}/src/Geometry.cpp                       \
            `root-config --cflags`        
                                       
${MonicelliDir}/Detector.o: ${MonicelliDir}/src/Detector.cpp       \
                            ${MonicelliDir}/include/ROC.h          \
                            ${MonicelliDir}/include/MessageTools.h \
                            ${MonicelliDir}/include/ANSIColors.h
	@echo ' '
	@echo '---------------- Compiling Detector ----------'
	g++ -c                                                     \
            -o ${MonicelliDir}/Detector.o                          \
            -I${MonicelliDir}/include                              \
            ${MonicelliDir}/src/Detector.cpp                       \
            `root-config --cflags`      
                                       
${MonicelliDir}/ROC.o: ${MonicelliDir}/src/ROC.cpp                 \
                       ${MonicelliDir}/include/MessageTools.h      \
                       ${MonicelliDir}/include/ANSIColors.h
	@echo ' '
	@echo '---------------- Compiling ROC ----------'
	g++ -c                                                     \
            -o ${MonicelliDir}/ROC.o                               \
            -I${MonicelliDir}/include                              \
            ${MonicelliDir}/src/ROC.cpp                            \
            `root-config --cflags`                                                                                                     
                                       
${MonicelliDir}/tmp/EventDict.o: ${MonicelliDir}/tmp/EventDict.C
	@echo ' '
	@echo '---------------- Compiling Event Dictionary ----------'
	g++ -c                                                     \
            -o ${MonicelliDir}/tmp/EventDict.o                     \
            ${MonicelliDir}/tmp/EventDict.C                        \
            `root-config --cflags`
        
${MonicelliDir}/tmp/EventDict.C: ${MonicelliDir}/include/Event.h   \
                                 ${MonicelliDir}/include/Geometry.h\
                                 ${MonicelliDir}/include/Detector.h\
                                 ${MonicelliDir}/include/ROC.h
	@echo ' '
	@echo '---------------- Building Event Dictionary ----------'
	rm -f     ${MonicelliDir}/tmp/EventDict.* &&               \
        rootcint  ${MonicelliDir}/tmp/EventDict.C                  \
                  -c ${MonicelliDir}/include/Event.h+              \
                     ${MonicelliDir}/include/Geometry.h+           \
                     ${MonicelliDir}/include/Detector.h+           \
                     ${MonicelliDir}/include/ROC.h+
        
clean: 
	rm -f *.o                                                  \
        $(PROGRAMS)                                                \
        ${MonicelliDir}/tmp/EventDict.C                            \
        ${MonicelliDir}/tmp/EventDict.h                            \
        ${MonicelliDir}/tmp/EventHeaderDict.C                      \
        ${MonicelliDir}/tmp/EventHeaderDict.h                      
