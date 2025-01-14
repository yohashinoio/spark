<div align="center">
  <h1>The Twinkle Programming Language</h1>

[Getting Started](docs/GettingStarted.md) |
[Language Reference](docs/LanguageReference.md) |
[Examples](examples)

</div>

## Features

- Object-oriented programming
- Generics
- Safe memory management with smart pointers (unstable)
- Tagged/Discriminated union (Like Rust's enum)
- LLVM is adopted for backend
- Variable shadowing
- Elixir-like pipeline operator
- Pascal-like pointer syntax

See the `examples` and `test/cases` folders for actual program examples.

## Installation

### Dependency List

- LLVM (Confirmed to work with 13 and 14)
- Boost
- CMake
- Make (Anything that is supported by CMake)
- C++20 compiler
- GCC

### Install Dependencies (Debian, Ubuntu)

Here is how to install them in ubuntu.

```bash
$ sudo apt update -y
```

Install GCC, Make and CMake.

```bash
$ sudo apt install -y build-essential cmake
```

Install Boost 1.80.0.

```bash
$ sudo apt install -y g++ python-dev-is-python3 autotools-dev libicu-dev libbz2-dev wget
$ cd /tmp
$ wget https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.bz2
$ tar jxvf boost_1_80_0.tar.bz2
$ cd boost_1_80_0/
$ ./bootstrap.sh --prefix=/usr/local/
$ sudo ./b2 install -j$(expr $(nproc) + 1)
```

Install LLVM 14. (https://apt.llvm.org/)<br/>
Versions other than 13 and 14 may not work.

```bash
$ sudo apt install lsb-release software-properties-common gnupg
$ cd /tmp
$ wget https://apt.llvm.org/llvm.sh
$ chmod +x llvm.sh
$ sudo ./llvm.sh 14
```

### Installation

Clone this repository.

```bash
$ git clone https://github.com/yohashinoio/twinkle.git
$ cd twinkle
```

Create build directory.

```bash
$ mkdir build && cd $_
```

Build and install. (If llvm-config command name is llvm-config-14)

```bash
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DLLVM_CONFIG_PATH="llvm-config-14"
$ cmake --build . -j
$ sudo cmake --install .
```

If you want to specify where to install.

```bash
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="path/to/install"
$ cmake --build . -j
$ cmake --install .
```

If you want to specify the path to llvm-config.

```bash
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DLLVM_CONFIG_PATH="path/to/llvm-config"
$ cmake --build . -j
$ sudo cmake --install .
```

### Testing

Passing -DENABLE_TEST=1 to cmake and building will create an executable file for testing.

```bash
$ cmake .. -DENABLE_TEST=1
$ cmake --build . -j
```

Run the test with CTest.

```bash
$ ctest -V
```

## Compiler Usage

If you want to compile and link `main.twinkle` and `sub.twinkle`.

```bash
$ twinkle main.twinkle sub.twinkle
$ ./a.out
```

If you want to do JIT compilation.

```bash
$ twinkle --JIT main.twinkle sub.twinkle
```

See help for more detailed description.

```bash
$ twinkle --help
```

## References

- [X3 Documentation](http://ciere.com/cppnow15/x3_docs/): Documentation for Boost.Spirit.X3.
- [Using X3 Slides](http://ciere.com/cppnow15/x3_docs/): Slides for Boost.Spirit.X3.
- [LLVM Tutorial](https://llvm.org/docs/GettingStartedTutorials.html): Tutorials about using LLVM. Includes a tutorial about making a custom language with LLVM.
- [LLVM Reference](https://llvm.org/docs/Reference.html): 'Doxygen generated documentation' and 'LLVM Language Reference Manual'
- [きつねさんでもわかる LLVM](https://tatsu-zine.com/books/llvm): あらゆる可能性を秘めたコンパイラ基盤として注目されている LLVM。本書はコンパイラを実際に作りながら LLVM のフロントエンドからバックエンドまでを幅広く解説した世界初(!?)の LLVM 解説本です。

## License

This project is available under the MIT license.<br/>
See LICENSE for the full content of the license.
