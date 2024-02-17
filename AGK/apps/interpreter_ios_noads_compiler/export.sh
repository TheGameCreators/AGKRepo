#! /bin/sh

cd /Users/paulj/Documents/SVN/AGKTrunk/apps/interpreter_ios_noads
lipo -create "Build/Intermediates/ArchiveIntermediates/agk_interpreter/IntermediateBuildFilesPath/agkinterpreter.build/Release-iphoneos/agk_interpreter.build/Objects-normal/armv7/AppGameKit Player" "Build/Intermediates/ArchiveIntermediates/agk_interpreter/IntermediateBuildFilesPath/agkinterpreter.build/Release-iphoneos/agk_interpreter.build/Objects-normal/arm64/AppGameKit Player" -output "../../IDE/Geany-1.24.1/data/ios/source/AppGameKit Player No Ads"
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip "../../IDE/Geany-1.24.1/data/ios/source/AppGameKit Player No Ads"
