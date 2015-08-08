wavtool-yawu
============

Yet Another Wavtool for UTAU

This is a drop-in replacement for the component `wavtool.exe` of [UTAU song synthesizer](http://utau2008.web.fc2.com/).

## Advantages

- Input WAV audio in sample formats at up to 64-bit float

- Output WAV audio in sample format at 32-bits

- Support sample rates other than 44100 Hz

- Robust design allows it to try to work even with incorrect parameters

## Building

Install [CMake](http://www.cmake.org/), at least version 3.0.

Install [MSYS2](https://msys2.github.io) on Windows or [MinGW-w64](http://mingw-w64.org/) toolchain on Linux.

Switch to the directory of `wavtool-yawu`, type:

```bash
./configure -DCMAKE_TOOLCHAIN_FILE=tools/toolchain-i686-w64-mingw32.cmake -DCMAKE_EXE_LINKER_FLAGS=-static -DUSE_BUNDLED_SNDFILE=1
make
```

The resulting executable file is in `build/wavtool-yawu.exe`.

## Usage

Open your project in UTAU, Select "Project" -> "Project Property" -> "Tool 1 (append)". Choose `wavtool-yawu.exe`.

## Usage with Cadencii

It is not recommended to use `wavtool-yawu` with [Cadencii](http://osdn.jp/projects/cadencii/) or [jCadencii](https://packages.debian.org/sid/main/jcadencii). That is because Cadencii tries to read a temporary file generated by wavtool, but `wavtool-yawu` never produces any garbage temporary file.

You can try [wavtool-pl](http://osdn.jp/projects/wavtool-pl/) instead.

If you really want to use with Cadencii, you have to configure and build `wavtool-yawu` with `-DYAWU_OUTPUT_SAMPLE_FORMAT=SF_FORMAT_PCM_16`.

After that, for Linux or OS X, open jCadencii, go to "Settings" -> "Preference" -> "Platform" -> "UTAU Cores" -> "wavtool" and select `cadencii-yawu`.

## Note

If you use UTAU with Wine on Linux, please check "Tools" -> "Option" -> "Rendering" -> "Note length calculated by GUI front-end". It is a known bug at UTAU side.

## License

This software is licensed under GNU Lesser General Public License version 3 and later. See the file [COPYING](COPYING) for more information.

