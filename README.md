# Fidesys Case Boundary Conditions Editor

Application is aimed to support researchers to easily modify some basic parameters of
the Fidesys project and recalculate it by pressing just one button (apart from being
test case for the job application).

Currently application supports modification of the following boundary conditions:
* point loads;
* face pressures;
* face distributed forces;
* displacements.

## Dependencies

* [nlohmann's json library](github.com/nlohmann/json)
* Qt6

### Obtaining dependencies

If you're familiar with the Conan2, then you can skip this entire section.
Otherwise, you should read it to be aware about what's happening while configuration proceedes.

[TL;DR]

Execute the following from the project's root directory:

```bash
$ mkdir build
$ conan profile detect --force
$ conan install . -of build --build=missing
```

[TL;DR]

This project uses [Conan2](https://conan.io) to manage dependencies.
Thus, if one wants to have it's dependencies managed automatically, he then should:
* have Conan2 installed on their system;
* have some conan profiles set up.

To let Conan detect profile which fits your system, try running the following:

```bash
$ conan profile detect -name <profile_name>
```

This command will write automatically detected settings into the file placed under
`$CONAN_HOME/profiles/<profile_name>`.

After that, you need conan to extract information about dependencies you wish to install
from `conanfile.txt` and install targets defined by the same file into some directory.
In order to do that, one should execute the following commands from the project's
root directory:

```bash
$ mkdir build
$ conan install . --output-folder=build --build=missing -pr:a <profile_name>
```

This will download missing dependencies from the remote repository,
install targets described in the `conanfile.txt` file under `build` directory
and build required binary artifacts if missing any using profile `<profile_name>`.

After that, everything is set up for the usual CMake configuration & build process:

```bash
$ cmake -S ./ -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=<Debug|Release>
$ cmake --build ./build/
```

Here we specify which toolchain CMake should use in order to build our project properly and which project configuration -
debug or release - we want to build.
Apart from that - nothing unusual for the average C/C++ developer.

## Idea behind the architecture

The overall idea I was following during the process of development is scalability (or the way how I see scalability) and relative
orthogonality (if application with graphical interface can ever have orthogonality).

Thus, there are classes like `Load`, `Pressure` and `Restraint` which generalize traits of loads (point of distributed), pressures
and restraints, respectively.

All of them inherit `JSONSerializable` interface which provides `deserialize` and `serialize` public pure virtual methods which
allow user to encapsulate this processes and evade copy/pasting the same code all over the place.

## Current functionality

Application supports:
* bullet-proof file loading;
* on-the-fly modifications saving into intermediate representation of the opened file (application writes all changes to RAM and write them at once on "Export" button hit);
* exporting changes into arbitrary file.
