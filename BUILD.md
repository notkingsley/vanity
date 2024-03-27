You'll need [cmake](https://cmake.org/) to build the project. You can install it with:
```bash
$ sudo apt-get install cmake
```

I suggest you use [ninja](https://ninja-build.org/) to build the project. You can install it with:
```bash
$ sudo apt-get install ninja-build
```
This is optional, but it's faster than make.

You'll also need a C++ compiler that supports C++20.  
Cmake will use the default compiler on your system. 

First clone the repo:
```bash
$ git clone https://github.com/notkingsley/vanity.git
$ cd vanity
```

Generate the build files:
```bash
$ cmake -DCMAKE_MAKE_PROGRAM=ninja -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -S ./vanity
```

Or if you want to use make:
```bash
$ cmake -B build -DCMAKE_BUILD_TYPE=Release -S ./vanity
```

Build the project:
```bash
$ cmake --build build --target vanity
```

This will create the `vanity` executable in the `build` directory. You can run it with:
```bash
$ ./build/vanity --port=9955 --port=19955 --working-dir=.vanity --no_logging
```
