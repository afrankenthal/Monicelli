PROJECT = Utilities
EXES     = bin/ReaderTemplate bin/Converter bin/GeometryConverter bin/T1037Converter bin/StripTestReader bin/AdamParaConverter

PWD         = `pwd`
ROOTCONFIG := root-config
ARCH       := $(shell $(ROOTCONFIG) --arch)
PLATFORM   := $(shell $(ROOTCONFIG) --platform)
64BITFLAG  := $(shell uname -m)
OBJDIR      = obj
BINDIR      = bin
INCDIR      = inc
SRCDIR      = src
LIBDIR      = lib
SRCEXT      = cpp

$(shell cp -p ../src/Detector.cpp ../src/ROC.cpp ../src/Event.cpp ../src/EventHeader.cpp ../src/Geometry.cpp $(SRCDIR))

OPTIMIZER_FLAGS = -g
#OPTIMIZER_FLAGS = -O3

INCFLAGS = -I$(INCDIR) \
           -I$(MonicelliDir)/include \
           `$(ROOTCONFIG) --cflags`
	   
LDLIBS = `$(ROOTCONFIG) --libs`

ifeq ($(PLATFORM),linux)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CC            = g++
CCFLAGS       = $(INCFLAGS) $(OPTIMIZER_FLAGS) -Wall -fPIC
  ifeq (${64BITFLAG},x86_64)
    CC        += -m64
  endif
LD            = $(CC)
LDFLAGS       = -O
SOFLAGS       = -shared
DllSuf        = so
endif

ifeq ($(PLATFORM),macosx)
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
CC            = c++
CCFLAGS       = $(INCFLAGS) -O2 -pipe -Wall -W -Woverloaded-virtual
LD            = $(MACOSXTARGET) c++
LDFLAGS       = -O2 -bind_at_load
DllSuf        = dylib
UNDEFOPT      = dynamic_lookup
LD            = c++
SOFLAGS       = -dynamiclib -single_module -undefined $(UNDEFOPT)
endif

SRCLIST   = $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJLIST   = $(patsubst $(SRCDIR)/%.$(SRCEXT),$(OBJDIR)/%.o,$(SRCLIST))
#SRCLIST  += $(MonicelliDir)/src/Detector.$(SRCEXT) $(MonicelliDir)/src/ROC.$(SRCEXT) $(MonicelliDir)/src/Geometry.$(SRCEXT)
#OBJLIST  += $(SRCDIR)/Detector.o $(SRCDIR)/ROC.o $(SRCDIR)/Geometry.o
#OBJLIST  += $(MonicelliDir)/Detector.o $(MonicelliDir)/ROC.o $(MonicelliDir)/Geometry.o
LIBLIST   = $(wildcard $(LIBDIR)/lib*.$(DllSuf))
NOPLUGINS = $(patsubst $(LIBDIR)/lib%Plugin.$(DllSuf), ,$(LIBLIST))
LDLIBLIST = $(patsubst $(LIBDIR)/lib%.$(DllSuf), -l%,$(NOPLUGINS))

LDLIBS += -L$(LIBDIR) $(LDLIBLIST)

DYNLIB  = $(LIBDIR)/lib$(PROJECT).$(DllSuf)

HERE   := $(shell pwd)

FORCED  = dep dynlib bin exes

.PHONY : all INTRO $(FORCED) EOJ clean cleanlib dep dynlib cleantilde clean$(OBJDIR)/dependencies.d

#----------------- Main sequence ----------------------#
all : INTRO PxEventDictionary $(FORCED) EOJ  
#------------------------------------------------------#

#*********************************************************************************************************
#--------------------------------------------------------------------------------------------------------#
INTRO : 
#	@clear
	@echo '[1;33m[1m     [7m' 'Compiling the $(PROJECT) in $(HERE) [0m' '[1;33m[1m[0m'
	@echo ' '
#*********************************************************************************************************

#--------------------------------------------------------------------------------------------------------#
dep :   
	@echo '     [1;32m[1m***** Making main sequence $(OBJDIR)/dependencies.d ******[0m'
	@if [ ! -e $(OBJDIR)/dependencies.d ] ; then touch $(OBJDIR)/dependencies.d ;fi
        ifdef CPPVERBOSE
	  g++ -MM $(INCDIR)/*.h $(SRCDIR)/*.$(SRCEXT) $(BINDIR)/*.$(SRCEXT) $(CCFLAGS) | sed 's/.*\.o:/$(OBJDIR)\/&/' >  $(OBJDIR)/dependencies.d
        else
	 @g++ -MM $(INCDIR)/*.h $(SRCDIR)/*.$(SRCEXT) $(BINDIR)/*.$(SRCEXT) $(CCFLAGS) | sed 's/.*\.o:/$(OBJDIR)\/&/' >  $(OBJDIR)/dependencies.d
        endif

#--------------------------------------------------------------------------------------------------------#
dynlib :  $(DYNLIB)
	
#--------------------------------------------------------------------------------------------------------#
exes : $(EXES)
	
#--------------------------------------------------------------------------------------------------------#
$(OBJDIR)/%.o : $(SRCDIR)/%.$(SRCEXT) 
	@echo '     [0;31m[1m[7mCompiling[0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	  $(CC) $(CCFLAGS) -c $< -o $@
        else
	 @$(CC) $(CCFLAGS) -c $< -o $@
        endif

#--------------------------------------------------------------------------------------------------------#
$(DYNLIB) : $(OBJLIST) $(OBJDIR)/EventDict.o $(OBJDIR)/EventHeaderDict.o
	@echo '     [1;32m[1m***** Making dynlib *****[0m'
        ifdef CPPVERBOSE
	  $(CC) $(SOFLAGS) -o $(DYNLIB) $(OBJLIST) obj/EventDict.o obj/EventHeaderDict.o
        else
	 @$(CC) $(SOFLAGS) -o $(DYNLIB) $(OBJLIST) obj/EventDict.o obj/EventHeaderDict.o
        endif
	@echo ' '
	@echo '     [0;35m[1m[7mMain sequence built[0m';
	@echo ' '

#--------------------------------------------------------------------------------------------------------#
$(BINDIR)/% : $(BINDIR)/%.$(SRCEXT) 
	@echo '     [0;31m[1m[7mBuilding   Executable [0m [0;36m[1m[7m$< [0m'
        ifdef CPPVERBOSE
	   $(CC) -o $@ $(CCFLAGS) $< $(LDLIBS) $(LDFLAGS) -Wl,-rpath=$(PWD)/$(LIBDIR)
        else
	  @$(CC) -o $@ $(CCFLAGS) $< $(LDLIBS) $(LDFLAGS) -Wl,-rpath=$(PWD)/$(LIBDIR)
        endif

#--------------------------------------------------------------------------------------------------------#
$(OBJDIR)/EventHeaderDict.o: $(OBJDIR)/EventHeaderDict.C
	@echo ' '
	@echo '------------- Compiling EventHeader Dictionary '
	g++ -c -O3 -fPIC                                                 \
            -o $(OBJDIR)/EventHeaderDict.o                               \
               $(OBJDIR)/EventHeaderDict.C                               \
               `root-config --cflags`
        
#--------------------------------------------------------------------------------------------------------#
$(OBJDIR)/EventHeaderDict.C: ${MonicelliDir}/include/EventHeader.h             
                       
	@echo ' '
	@echo '------------- Building EventHeader Dictionary '
	rm -f     $(OBJDIR)/EventHeaderDict.* &&                         \
        rootcint  $(OBJDIR)/EventHeaderDict.C                            \
                  -c -O3 -fPIC ${MonicelliDir}/include/EventHeader.h+        

#--------------------------------------------------------------------------------------------------------#
$(OBJDIR)/EventDict.o: $(OBJDIR)/EventDict.C
	@echo ' '
	@echo '------------- Compiling Event Dictionary '
	g++ -c -O3 -fPIC                                           \
            -o $(OBJDIR)/EventDict.o                               \
               $(OBJDIR)/EventDict.C                               \
               `root-config --cflags`
        
#--------------------------------------------------------------------------------------------------------#
$(OBJDIR)/EventDict.C: ${MonicelliDir}/include/Event.h             \
                       ${MonicelliDir}/include/Geometry.h          \
                       ${MonicelliDir}/include/Detector.h          \
                       ${MonicelliDir}/include/ROC.h
	@echo ' '
	@echo '------------- Building Event Dictionary '
	rm -f     $(OBJDIR)/EventDict.* &&                         \
        rootcint  $(OBJDIR)/EventDict.C                            \
                  -c -O3 -fPIC ${MonicelliDir}/include/Event.h+    \
                     ${MonicelliDir}/include/Geometry.h+           \
                     ${MonicelliDir}/include/Detector.h+           \
                     ${MonicelliDir}/include/ROC.h+
###################################################################################################
PxEventDictionary: $(INCDIR)/PxEvent.h 
	cd $(INCDIR); rootcint -f $@.$(SRCEXT) -c PxEvent.h+; mv $@.$(SRCEXT) ../$(SRCDIR)/
#--------------------------------------------------------------------------------------------------------#
install:
	@echo '     [0;31m[1m[7mInstalling Executable [0m'
        ifdef CPPVERBOSE
	   mv $(BINDIR)/$(EXE) $(PROJECT_BIN)
        else
	  @mv $(BINDIR)/$(EXE) $(PROJECT_BIN)
        endif
	
#--------------------------------------------------------------------------------------------------------#
clean : 
	@echo '[1;32m[1m*****************************************************************[0m'
	@echo '[1;32m[1m----------- [0;31m[1m[7mMain sequence cleanup [0m' '[1;32m[1m------------------------------[0m'
	@make cleansmallprint
	@echo '[1;32m[1m----------- [0;31m[1m[7mDone [0m' '[1;32m[1m-----------------------------------------------[0m'
	@echo '[1;32m[1m*****************************************************************[0m'

#--------------------------------------------------------------------------------------------------------#
cleansmallprint : 
	@echo '[1;32m[1m            Main sequence clean up                               [0m'
	@make cleantilde cleanlib cleandependencies cleanbin

#--------------------------------------------------------------------------------------------------------#
cleantilde :
        ifdef CPPVERBOSE
	  rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.h $(OBJDIR)/*.C $(INCDIR)/*~ $(SRCDIR)/*~ $(BINDIR)/*~
        else
	 @rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.h $(OBJDIR)/*.C $(INCDIR)/*~ $(SRCDIR)/*~ $(BINDIR)/*~
        endif

#--------------------------------------------------------------------------------------------------------#
cleanlib : 
	@echo '[1;32m[1m            Dynamic library clean up                             [0m'
        ifdef CPPVERBOSE
	  rm -rf $(DYNLIB)
        else
	 @rm -rf $(DYNLIB)
        endif

#--------------------------------------------------------------------------------------------------------#
cleanbin : 
	@echo '[1;32m[1m            Executable clean up                             [0m'
        ifdef CPPVERBOSE
	  rm -f $(EXES)
        else
	 @rm -f $(EXES)
        endif

#--------------------------------------------------------------------------------------------------------#
cleandependencies :
        ifdef CPPVERBOSE
	  rm -rf $(OBJDIR)/dependencies.d
        else
	 @rm -rf $(OBJDIR)/dependencies.d
        endif

#--------------------------------------------------------------------------------------------------------#
EOJ : 
	@echo ' '
	@echo '     [0;35m[1m[7m$(PROJECT) done!![0m';
	@echo ' '
	@echo ' '

#--------------------------------------------------------------------------------------------------------#
-include $(OBJDIR)/dependencies.d
