# CMAKE generated file: DO NOT EDIT!
# Generated by "Borland Makefiles" Generator, CMake Version 3.22

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force: NUL
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\work\BAISim\3rdparty\zlib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\work\BAISim\3rdparty\zlib

# Include any dependencies generated for this target.
!include CMakeFiles\example.dir\depend.make
# Include any dependencies generated by the compiler for this target.
!include CMakeFiles\example.dir\compiler_depend.make

# Include the progress variables for this target.
!include CMakeFiles\example.dir\progress.make

# Include the compile flags for this target's objects.
!include CMakeFiles\example.dir\flags.make

CMakeFiles\example.dir\test\example.c.obj: CMakeFiles\example.dir\flags.make
CMakeFiles\example.dir\test\example.c.obj: CMakeFiles\example.dir\includes_C.rsp
CMakeFiles\example.dir\test\example.c.obj: test\example.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\work\BAISim\3rdparty\zlib\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/example.dir/test/example.c.obj"
	C:\PROGRA~2\EMBARC~1\Studio\21.0\bin\bcc32c.exe -tR -DWIN32 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -oCMakeFiles\example.dir\test\example.c.obj -c C:\work\BAISim\3rdparty\zlib\test\example.c

CMakeFiles\example.dir\test\example.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/example.dir/test/example.c.i"
	cpp32 -DWIN32 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -oCMakeFiles\example.dir\test\example.c.i -c C:\work\BAISim\3rdparty\zlib\test\example.c

CMakeFiles\example.dir\test\example.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/example.dir/test/example.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

# Object files for target example
example_OBJECTS = \
"CMakeFiles\example.dir\test\example.c.obj"

# External object files for target example
example_EXTERNAL_OBJECTS =

example.exe: CMakeFiles\example.dir\test\example.c.obj
example.exe: CMakeFiles\example.dir\build.make
example.exe: zlib.lib
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\work\BAISim\3rdparty\zlib\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable example.exe"
	C:\PROGRA~2\EMBARC~1\Studio\21.0\bin\bcc32c.exe -tR -eexample.exe -tM -lS:1048576 -lSc:4098 -lH:1048576 -lHc:8192 -v -tC -tM -Od -v @&&|
  zlib.lib import32.lib  $(example_OBJECTS) $(example_EXTERNAL_OBJECTS)
|

# Rule to build all files generated by this target.
CMakeFiles\example.dir\build: example.exe
.PHONY : CMakeFiles\example.dir\build

CMakeFiles\example.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\example.dir\cmake_clean.cmake
.PHONY : CMakeFiles\example.dir\clean

CMakeFiles\example.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "Borland Makefiles" C:\work\BAISim\3rdparty\zlib C:\work\BAISim\3rdparty\zlib C:\work\BAISim\3rdparty\zlib C:\work\BAISim\3rdparty\zlib C:\work\BAISim\3rdparty\zlib\CMakeFiles\example.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\example.dir\depend

