# MCX Injector
Reverse engineering of the MechCommander (Gold) source code.

# Setup

1. Download and build https://github.com/microsoft/Detours **for x86**
2. Setup an environment variable named `MCXI_DETOURS_PATH` to the folder where Detours `include` and `lib.x86` folder reside
3. Configure a debug env. var (or system env. var) named `MCXI_EXE_PATH` that points to the full path of the MechCommander executable (`MCX.EXE`)
4. Run the `MCXLauncher` project
