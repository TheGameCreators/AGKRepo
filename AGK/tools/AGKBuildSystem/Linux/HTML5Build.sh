#! /bin/bash

source ~/Downloads/emsdk_portable/emsdk_env.sh

set -e
cd ../../..
#make clean -f Makefile_html5 # do this separately
make -j2 -f Makefile_html5

# 3D static memory
cd apps/interpreter_html5
make clean
make
cp AGKPlayer.asm.js ../../AgkIde/media/data/html5/3D/AGKPlayer.asm.js
cp AGKPlayer.html ../../AgkIde/media/data/html5/3D/AGKPlayer.html
cp AGKPlayer.html.mem ../../AgkIde/media/data/html5/3D/AGKPlayer.html.mem
cp AGKPlayer.js ../../AgkIde/media/data/html5/3D/AGKPlayer.js

# 3D static memory
make DYNAMIC=1
cp AGKPlayer.asm.js ../../AgkIde/media/data/html5/3Ddynamic/AGKPlayer.asm.js
cp AGKPlayer.html ../../AgkIde/media/data/html5/3Ddynamic/AGKPlayer.html
cp AGKPlayer.html.mem ../../AgkIde/media/data/html5/3Ddynamic/AGKPlayer.html.mem
cp AGKPlayer.js ../../AgkIde/media/data/html5/3Ddynamic/AGKPlayer.js

# 2D static memory
make clean
make 2D=1
cp AGKPlayer.asm.js ../../AgkIde/media/data/html5/2D/AGKPlayer.asm.js
cp AGKPlayer.html ../../AgkIde/media/data/html5/2D/AGKPlayer.html
cp AGKPlayer.html.mem ../../AgkIde/media/data/html5/2D/AGKPlayer.html.mem
cp AGKPlayer.js ../../AgkIde/media/data/html5/2D/AGKPlayer.js

# 2D dynamic memory
make 2D=1 DYNAMIC=1
cp AGKPlayer.asm.js ../../AgkIde/media/data/html5/2Ddynamic/AGKPlayer.asm.js
cp AGKPlayer.html ../../AgkIde/media/data/html5/2Ddynamic/AGKPlayer.html
cp AGKPlayer.html.mem ../../AgkIde/media/data/html5/2Ddynamic/AGKPlayer.html.mem
cp AGKPlayer.js ../../AgkIde/media/data/html5/2Ddynamic/AGKPlayer.js

