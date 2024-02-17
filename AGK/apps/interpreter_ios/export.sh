#! /bin/sh

cd /Users/paulj/Documents/SVN/AGKTrunk/apps/interpreter_ios
lipo -create "Build/Intermediates/ArchiveIntermediates/agk_interpreter/IntermediateBuildFilesPath/agkinterpreter.build/Release-iphoneos/agk_interpreter.build/Objects-normal/armv7/AppGameKit Player" "Build/Intermediates/ArchiveIntermediates/agk_interpreter/IntermediateBuildFilesPath/agkinterpreter.build/Release-iphoneos/agk_interpreter.build/Objects-normal/arm64/AppGameKit Player" -output "../../IDE/Geany-1.24.1/data/ios/source/AppGameKit Player.app/AppGameKit Player"
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip "../../IDE/Geany-1.24.1/data/ios/source/AppGameKit Player.app/AppGameKit Player"
