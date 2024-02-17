# discover if this is a 32 or 64 bit machine
ARCH := $(shell getconf LONG_BIT)

CC = g++
ODIR = build/obj$(ARCH)
INC = -I../../common/include -I../../common -I../interpreter -I../../bullet -I../../bullet/BulletCollision/CollisionShapes -I../../renderer
CFLAGS = -O2 -std=c++11
LDFLAGS32 = -L../../platform/linux/Lib/Release32
LDFLAGS64 = -L../../platform/linux/Lib/Release64
LDFLAGS = $(LDFLAGS$(ARCH)) -static-libstdc++ -static-libgcc

# list of dependencies so that changes to these files cause a rebuild of source files
DEPS = template.h

# list of object files to compile, they will be placed in the $(ODIR) folder
# matching .cpp files will be searched for below
_OBJS = Core.o \
        template.o  
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

# entry point, create folders, compile source files, and link the executable
all: setup $(OBJS) Executable

# make sure the build folders exist
setup:
	mkdir -p build/obj$(ARCH)

# compile a source file, first search path is the template folder
$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -DIDE_LINUX -static -c $(INC) -o $@ $< $(CFLAGS)

# second search path is the interpreter folder, used as an example, not used
$(ODIR)/%.o: ../interpreter/%.cpp $(DEPS)
	$(CC) -DIDE_LINUX -static -c $(INC) -o $@ $< $(CFLAGS)

# link the .o files
Executable:
	$(CC) $(OBJS) -o build/LinuxApp$(ARCH) $(LDFLAGS) -Wl,--unresolved-symbols=ignore-all -Wl,-Bstatic -lAGKLinux -Wl,-Bdynamic -lpthread -ludev -no-pie

# delete compiled files
clean:
	rm -rf build/obj$(ARCH)/*
	rm -f build/LinuxApp$(ARCH)
