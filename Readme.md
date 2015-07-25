wavtool-yawu
============

Yet Another Wavtool UTAU

This is a drop-in replacement for the component `wavtool.exe` of [UTAU song synthesizer](http://utau2008.web.fc2.com/).

## Advantage

- Input WAV audio in sample format at up to 64-bit float

- Output WAV audio in sample format at 32-bit

- Support sample rates other than 44100 Hz

- Robust design makes it not easily to crash

## Building

Install [CMake](http://www.cmake.org/), at least version 3.0.

Install [MSYS2](https://msys2.github.io) on Windows or [MinGW-w64](http://mingw-w64.org/) toolchain on Linux.

Switch to the directory of `wavtool-yawu`, type:

```bash
./configure -DCMAKE_TOOLCHAIN_FILE=tools/toolchain-i686-w64-mingw32.cmake -DCMAKE_EXE_LINKER_FLAGS=-static
make
```

The resulting executable file is in `build/wavtool-yawu.exe`.

## Usage

Open your project in UTAU, Select "Project" -> "Project Property" -> "Tool 1 (append)". Choose `wavtool-yawu.exe`.

You can also use `wavtool-yawu` with [Cadencii](http://osdn.jp/projects/cadencii/) or [jCadencii](https://packages.debian.org/sid/main/jcadencii).

## Note

If you use UTAU with Wine on Linux, please check "Tools" -> "Option" -> "Rendering" -> "Note length calculated by GUI front-end". It is a known bug at UTAU side.

## License

This software is licensed under GNU Lesser General Public License version 3 and later. See the file [COPYING] for more information.
