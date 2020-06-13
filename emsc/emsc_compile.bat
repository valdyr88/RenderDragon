cmake -DCMAKE_TOOLCHAIN_FILE=C:/WebAssembly/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "MinGW Makefiles"
mingw32-make
xcopy /s "emsc_out\bin\*.*" "..\" /Y
xcopy /s "index.html" "..\" /Y