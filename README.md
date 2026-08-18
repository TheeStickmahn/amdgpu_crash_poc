## AMDGPU GPUVM Crash PoC
This repository contains a small SDLGPU application that triggers an GPUVM protection fault in AMDGPU on Linux.
It has been tested on both an RX 6700XT and a 780M. Both were ran with the AMDGPU driver, on updated versions of Fedora 44 (6700XT) and Arch Linux (780M).
Fedora was running on kernel `Linux 7.1.5-200.fc44.x86_64`, while Arch Linux was running on `Linux 7.1.8.1-cachyos`

This behaviour was displayed on both GNOME & KDE Plasma. (both Wayland)

Notably, this only causes a system lockup if ran through XWayland. Running it on Wayland still threw a fault, however the system continued on as normal.

### Compiling
Run ```cc amdgpu_crash_poc.c -lSDL3 -o pcblowerupper6000``` and then run the compiled program. (You'll obviously have to have SDL3 installed.)

To run the program through XWayland you can set the `SDL_VIDEO_DRIVER` environment variable to `x11` like this: `SDL_VIDEO_DRIVER=x11 pcblowerupper6000`

The `shaders.h` header contains the SPIR-V shaders as uint8_t arrays, but the HLSL that compiled into the embedded shaders is also in the repository. 
I *do* recommend compiling them locally on your machine, simply to remove any unforseen variables, but it should compile to byte-identical SPIR-V either way.

# Obvious disclaimer!!
You will be running a bit of software specifically designed **_to crash your OS through invalid usage of your GPU!_**

This has never damaged any hardware I've triggered it on, ever, **_but I can't guarantee it won't damage yours!_**
<br>This is a piece of software that, from user-level, can easily lock up an entire Linux system, by simply forgetting to submit a few Vulkan commands. This is **NOT** intended behaviour.

Exercise **extreme caution.**

-- TheStickmahn (thestickmahn@proton.me)
