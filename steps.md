cmake .. -DCMAKE_INSTALL_PREFIX=.
make -j4
cpack # this will create a lib directory inside "build/out/_CPack_Packages/Darwin/DragNDrop/chordcat-0.1.0-Darwin/ALL_IN_ONE/lib"
# copy this folder into build/chordcat.app/Contents/ and rename it to Frameworks
run cpack again

## that didnt entirely work so Manual Transmission from here onwards:
check the otool -L links on the cpack generated executable inside Contents/MacOS
there are a bunch of system local references like FluidSynth.framework/Versions/3/FluidSynth 
and /opt/homebrew/opt/jack/lib/libjack.0.1.0.dylib

I copied a libjack.0.1.0.dylib and a libfluidsynth.3.dylib into the Frameworks folder and
from the build folder I ran:

`install_name_tool -change FluidSynth.framework/Versions/3/FluidSynth @rpath/libfluidsynth.3.dylib out/_CPack_Packages/Darwin/DragNDrop/chordcat-0.1.0-Darwin/ALL_IN_ONE/chordcat.app/Contents/MacOS/chordcat`
`install_name_tool -change /opt/homebrew/opt/jack/lib/libjack.0.1.0.dylib @rpath/libjack.0.1.0.dylib out/_CPack_Packages/Darwin/DragNDrop/chordcat-0.1.0-Darwin/ALL_IN_ONE/chordcat.app/Contents/MacOS/chordcat`

Then I deleted all that bin, lib and share folders from the ALL_IN_ONE dir

After that I opened Disk Utility and did File -> New Image -> Image From Folder.

Selected that folder `ALL_IN_ONE` which had `chordcat.app` and a symlink to applications