#! /bin/sh

set -e
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial
rm -rf /home/paul/Programs/AGKStudioLinuxTrial/*

# copy files from Windows build
echo "  Copying from Windows shared folder"
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Tier2/apps/template_linux
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Tier2/bullet
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Tier2/common
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Tier2/platform/linux
cp -r /home/paul/Shares/FilesAlphaLinux/AGK/Tier2/apps/template_linux/* /home/paul/Programs/AGKStudioLinuxTrial/Tier2/apps/template_linux/
cp -r /home/paul/Shares/FilesAlphaLinux/AGK/Tier2/bullet/* /home/paul/Programs/AGKStudioLinuxTrial/Tier2/bullet/
cp -r /home/paul/Shares/FilesAlphaLinux/AGK/Tier2/common/* /home/paul/Programs/AGKStudioLinuxTrial/Tier2/common/
cp -r /home/paul/Shares/FilesAlphaLinux/AGK/Tier2/platform/linux/* /home/paul/Programs/AGKStudioLinuxTrial/Tier2/platform/linux/

# copy readme and change log
cp /home/paul/Shares/FilesAlphaLinux/AGK/ReadMe /home/paul/Programs/AGKStudioLinuxTrial/ReadMe
cp /home/paul/Shares/FilesAlphaLinux/AGK/ChangeLog.txt /home/paul/Programs/AGKStudioLinuxTrial/ChangeLog.txt

# copy help files
echo "  Copying help files"
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Help
cp -r /home/paul/Shares/FilesAlphaLinux/AGK/Tier1/Help/* /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Help/

# copy projects
echo "  Copying projects"
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Projects
cp -r Examples/* /home/paul/Programs/AGKStudioLinuxTrial/Projects/

# remove export files from IDE folder
echo "  Removing Android files from IDE"
rm -rf /home/paul/Programs/AGK/Tier1/Editor/share/geany/android/*
echo "  Removing iOS files from IDE"
rm -rf /home/paul/Programs/AGK/Tier1/Editor/share/geany/ios/*

# copy compilers and command list
echo "  Copying compiler and broadcaster"
mkdir -p /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Compiler/interpreters
cp CompilerNew/build/AGKCompiler64 /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Compiler/AGKCompiler64
cp CompilerNew/CommandList.dat /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Compiler/CommandList.dat

# copy broadcasters
cp Broadcaster/AGKBroadcaster/build/AGKBroadcaster64 /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Compiler/AGKBroadcaster64

# copy interpreters
cp apps/interpreter_linux/build/LinuxPlayer64 /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Compiler/interpreters/LinuxPlayer64

# copy AGK libs
echo "  Copying Tier 2 libs"
cp platform/linux/Lib/Release64Trial/libAGKLinux.a /home/paul/Programs/AGKStudioLinuxTrial/Tier2/platform/linux/Lib/Release64/libAGKLinux.a

# copy IDE to build folder, separate executable into 32 and 64 bit versions
echo "  Copying IDE"
cp -r /home/paul/Programs/AGK/Tier1/* /home/paul/Programs/AGKStudioLinuxTrial/Tier1/
mv /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Editor/bin/geany /home/paul/Programs/AGKStudioLinuxTrial/Tier1/Editor/bin/geany64

# create shortcuts for IDE executable
echo "  Creating shortcuts"
cd /home/paul/Programs/AGKStudioLinuxTrial
ln -s Tier1/Editor/bin/geany64 AGK
