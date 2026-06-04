![Game image](./sample.png)

This is a C++ rewrite of FlightSim.  
Since it's no longer tied to Python, it doesn't need PyOpenGL support!  
  
The actual work was done by AI.  
Design and implementation: Claude 4.5 Sonnet  
Debugging: GPT-5 Thinking  

## The background:  
  
While chatting with an AI, I asked it if it was possible to install the latest NetBSD on an old Pentium-M machine and then run an application using Python 3, Pygame, and PyOpenGL.
The answer was that the latest PyOpenGL doesn't support Pentium-M, making it practically impossible.
At that point, the AI ​​suggested, "Why not port the application from Python to C++?"

??? That's it! With the help of the AI ​​now, porting from Python to C++ might be something I could do in no time!
And that's exactly what this project is.
The porting went smoothly, and including debugging, it was finished in about 10 hours.
Amazing, isn't it?  
  
# Library Installation:

## On Linux:
```bash
sudo apt install libglfw3-dev libglew-dev libglu1-mesa-dev freeglut3-dev
```

## On NetBSD:
Configure /etc/mk.conf
```bash
# Explicitly specify gcc-8.3 with full path
GCC_REQD=8
PKG_CC=/usr/pkg/gcc8/bin/gcc
PKG_CXX=/usr/pkg/gcc8/bin/g++
PKG_CPP=/usr/pkg/gcc8/bin/cpp

# Use the gcc-8.3 linker as well
PKG_LDFLAGS+=-L/usr/pkg/gcc8/lib
```
```bash
su
# Build GLFW
cd /usr/pkgsrc/graphics/glfw
make install clean

# Build GLEW
cd /usr/pkgsrc/graphics/glew
make install clean
exit
```

## On Windows:

On Windows, use w64devkit.
w64devkit is located in C:\w64devkit.

#### 1. Installing GLFW3

1. Download the latest `glfw-3.x.x.bin.WIN64.zip` from https://github.com/glfw/glfw/releases
2. After extraction:

- `include/GLFW/*` → `C:\w64devkit\x86_64-w64-mingw32\include\GLFW\`

- `lib-mingw-w64/*.a` → `C:\w64devkit\x86_64-w64-mingw32\lib\`

- `lib-mingw-w64/*.dll` → `C:\w64devkit\bin\`

#### 2. Installing GLEW

1. Download `glew-x.x.x-win32.zip` from https://github.com/nigels-com/glew/releases
2. After unzipping:
- `include/GL/*` → `C:\w64devkit\x86_64-w64-mingw32\include\GL\`
- Rename `lib/Release/x64/glew32.lib` to `glew32.a` → `C:\w64devkit\x86_64-w64-mingw32\lib\`
- `bin/Release/x64/glew32.dll` → `C:\w64devkit\bin\`

**Alternatively, use the MinGW binary:**
```bash
# Convert lib files to a files (if necessary)
cd /c/w64devkit/x86_64-w64-mingw32/lib
# If an .a file already exists, you can use it as is
```

#### 3. Installing GLM (Header Only)

1. Download the latest `.zip` from https://github.com/g-truc/glm/releases
2. After extraction:
- `glm/glm/*` → `C:\w64devkit\x86_64-w64-mingw32\include\glm\`


