CC = emcc
ODIR = build/obj
INC = -I../../common/include -I../../common -I../interpreter -I../../bullet -I../../bullet/BulletCollision/CollisionShapes -I../../renderer
CFLAGS = -O2 -std=c++11 -DDISABLE_VULKAN
ifeq ($(2D),1)
	CFLAGS += -DAGK_2D_ONLY
endif
LDFLAGS = -L../../platform/html5/Lib/Release
ifeq ($(DYNAMIC),1)
	LDFLAGS += -s ALLOW_MEMORY_GROWTH=1
else 
	LDFLAGS += -s TOTAL_MEMORY=268435456
endif
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

setup:
	mkdir -p build/obj

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -DIDE_HTML5 -c $(INC) -o $@ $< $(CFLAGS)
	
$(ODIR)/%.o: ../interpreter/%.cpp $(DEPS)
	$(CC) -DIDE_HTML5 -c $(INC) -o $@ $< $(CFLAGS)
	
AGKPlayer:
	$(CC) $(OBJS) -o AGKPlayer.html $(LDFLAGS) -O2 --shell-file ../../platform/html5/Source/agkshell.html --separate-asm -s USE_GLFW=3 -s FULL_ES2=1 -s USE_SDL=1 -s OUTLINING_LIMIT=20000 -s DISABLE_EXCEPTION_CATCHING=0 -lAGKHTML5 --preload-file media/bytecode.byc

clean:
	rm -rf build/obj/*
	rm -f AGKPlayer.html
