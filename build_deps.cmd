cd external/detours
nmake
cd ../..

cd external/abseil
rmdir /s /q build
mkdir build
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DABSL_BUILD_TESTING=OFF -DABSL_USE_GOOGLETEST_HEAD=OFF -DCMAKE_CXX_STANDARD=17
cmake --build build --target build
cd ../..