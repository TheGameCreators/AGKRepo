ARCH := $(shell getconf LONG_BIT)

CC = g++
ODIR = build/obj$(ARCH)
INC = -I../../common/include -I../../common -I../interpreter -I../../bullet -I../../bullet/BulletCollision/CollisionShapes -I../../renderer

CFLAGS = -O2 -std=c++11
LDFLAGS32 = -L../../platform/linux/Lib/Release32 
LDFLAGS64 = -L../../platform/linux/Lib/Release64 
LDFLAGS = -static-libstdc++ -static-libgcc 
DEPS = ../interpreter/interpreter.h \
       ../interpreter/AGKCommandEnums.h \
       ../interpreter/AGKCommandSwitch.h \
       ../interpreter/AGKCommonSwitch.h \
       ../interpreter/ProgramData.h

_OBJS = Core.o \
        interpreter.o \
        ProgramData.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

all: setup $(OBJS) AGKPlayer

trial: trialflags all
	
trialflags:
	$(eval CFLAGS += -DFREEVERSION)
	$(eval LDFLAGS32 = -L../../platform/linux/Lib/Release32Trial)
	$(eval LDFLAGS64 = -L../../platform/linux/Lib/Release64Trial)

setup:
	mkdir -p build/obj$(ARCH)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -DIDE_LINUX -c $(INC) -o $@ $< $(CFLAGS)
	
$(ODIR)/%.o: ../interpreter/%.cpp $(DEPS)
	$(CC) -DIDE_LINUX -c $(INC) -o $@ $< $(CFLAGS)
	
AGKPlayer: | $(OBJS)
	$(CC) $(LDFLAGS$(ARCH)) $(LDFLAGS) $(OBJS) -o build/LinuxPlayer$(ARCH) $(LDFLAGS) -Wl,-Bstatic -lAGKLinux -lglfw3 -Wl,-Bdynamic -lGL -lXt -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor -lopenal -ludev -ldl -no-pie 

clean:
	rm -rf build/obj$(ARCH)/*
	rm -f build/LinuxPlayer$(ARCH)
