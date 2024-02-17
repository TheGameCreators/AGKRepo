ARCH := $(shell getconf LONG_BIT)

CC = g++
ODIR = build/obj$(ARCH)
INC = 
LDFLAGS = 
CFLAGS = -O2 -std=c++11

_OBJS = AGKCompiler2.o \
        cFile.o \
        DynArray.o \
        Parser1.o \
        Parser2.o \
        ProgramData.o \
        LinuxCompiler.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

all: setup $(OBJS) Executable

trial: trialflags all
	
trialflags:
	$(eval CFLAGS += -DFREEVERSION)
	
weekend: weekendflags all

weekendflags:
	$(eval CFLAGS += -DAGK_STEAM_WEEKEND)
	$(eval INC += -I /home/paul/Downloads/sdk/public)
	$(eval LDFLAGS += -L/home/paul/Downloads/sdk/redistributable_bin/linux64 -lsteam_api -Wl,-rpath='$$$$ORIGIN')

static: staticflags setup $(OBJS) linkall

staticflags:
	$(eval CFLAGS += -DAGK_STATIC_LIB)

staticide: staticideflags setup $(OBJS) linkallide

staticideflags:
	$(eval CFLAGS += -DAGK_STATIC_LIB_IDE -DAGK_STATIC_LIB)

setup:
	mkdir -p $(ODIR)

$(ODIR)/%.o: %.cpp
	$(CC) -DIDE_LINUX -DAGK_COMPILER -no-pie -c $(INC) $(CFLAGS) -o $@ $<
	
$(ODIR)/%.o : ../common/Source/%.cpp
	$(CC) -DIDE_LINUX -DAGK_COMPILER -no-pie -c $(INC) $(CFLAGS) -o $@ $<
	
Executable: | $(OBJS)
	$(CC) $(OBJS) -o build/AGKCompiler$(ARCH) $(LDFLAGS) -L../platform/linux/Lib/Release$(ARCH) -lAGKLinux
	
linkall: | $(OBJS)
	ar rcs build/libAGKCompiler$(ARCH).a $(OBJS)

linkallide: | $(OBJS)
	ar rcs build/libAGKCompilerIDE$(ARCH).a $(OBJS)

clean:
	rm -rf build/obj$(ARCH)/*
	rm -f build/AGKCompiler$(ARCH)
	rm -f build/libAGKCompiler$(ARCH).a
	rm -f build/libAGKCompilerIDE$(ARCH).a
