ARCH := $(shell getconf LONG_BIT)

CC = g++
ODIR = build/obj$(ARCH)
INC = 
CFLAGS = -O2 -std=c++11

_OBJS = LinuxCore.o \
        Listener.o \
        Device.o \
        AGKBroadcaster.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

all: executableflags setup $(OBJS) Executable

executableflags:
	$(eval INC = -I../../common/include -I../../common -I../../bullet -I../../bullet/BulletCollision/CollisionShapes -I../../renderer)
	$(eval ODIR = build/obj$(ARCH))

static: staticflags setup $(OBJS) linkall

staticflags:
	$(eval CFLAGS += -DAGK_STATIC)
	$(eval INC = -I../../common/include -I../../common -I../../bullet -I../../bullet/BulletCollision/CollisionShapes -I../../renderer)

staticide: staticideflags setup $(OBJS) linkallide

staticideflags:
	$(eval CFLAGS += -DAGK_STATIC)
	$(eval INC = -I../../AgkIde/common/include -I../../AgkIde/common -I../../AgkIde/bullet -I../../AgkIde/bullet/BulletCollision/CollisionShapes -I../../renderer)

setup:
	mkdir -p $(ODIR)

$(ODIR)/%.o: %.cpp
	$(CC) -DAGK_LINUX -no-pie -c $(INC) -o $@ $< $(CFLAGS)
	
Executable: | $(OBJS)
	$(CC) $(OBJS) -o build/AGKBroadcaster$(ARCH) -L../../platform/linux/Lib/Release$(ARCH) -Wl,-Bstatic -lAGKLinux -lglfw3 -Wl,-Bdynamic -lGL -lXt -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor -lopenal -ludev -ldl

linkall: | $(OBJS)
	ar rcs build/libAGKBroadcaster$(ARCH).a $(OBJS)

linkallide: | $(OBJS)
	ar rcs build/libAGKBroadcasterIDE$(ARCH).a $(OBJS)

clean:
	rm -rf build/obj$(ARCH)/*
	rm -f build/AGKBroadcaster$(ARCH)
	rm -f build/libAGKBroadcaster$(ARCH).a
	rm -f build/libAGKBroadcasterIDE$(ARCH).a
