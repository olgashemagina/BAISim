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
CMAKE_SOURCE_DIR = C:\work\BAISim\3rdparty\libpng

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\work\BAISim\3rdparty\libpng

# Include any dependencies generated for this target.
!include CMakeFiles\pngfix.dir\depend.make
# Include any dependencies generated by the compiler for this target.
!include CMakeFiles\pngfix.dir\compiler_depend.make

# Include the progress variables for this target.
!include CMakeFiles\pngfix.dir\progress.make

# Include the compile flags for this target's objects.
!include CMakeFiles\pngfix.dir\flags.make

CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.obj: CMakeFiles\pngfix.dir\flags.make
CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.obj: CMakeFiles\pngfix.dir\includes_C.rsp
CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.obj: contrib\tools\pngfix.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\work\BAISim\3rdparty\libpng\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/pngfix.dir/contrib/tools/pngfix.c.obj"
	C:\PROGRA~2\EMBARC~1\Studio\21.0\bin\bcc32c.exe -tR -DWIN32 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -oCMakeFiles\pngfix.dir\contrib\tools\pngfix.c.obj -c C:\work\BAISim\3rdparty\libpng\contrib\tools\pngfix.c

CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/pngfix.dir/contrib/tools/pngfix.c.i"
	cpp32 -DWIN32 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -oCMakeFiles\pngfix.dir\contrib\tools\pngfix.c.i -c C:\work\BAISim\3rdparty\libpng\contrib\tools\pngfix.c

CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/pngfix.dir/contrib/tools/pngfix.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

# Object files for target pngfix
pngfix_OBJECTS = \
"CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.obj"

# External object files for target pngfix
pngfix_EXTERNAL_OBJECTS =

pngfix.exe: CMakeFiles\pngfix.dir\contrib\tools\pngfix.c.obj
pngfix.exe: CMakeFiles\pngfix.dir\build.make
pngfix.exe: png16d.lib
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\work\BAISim\3rdparty\libpng\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable pngfix.exe"
	C:\PROGRA~2\EMBARC~1\Studio\21.0\bin\bcc32c.exe -tR -epngfix.exe -tM -lS:1048576 -lSc:4098 -lH:1048576 -lHc:8192 -v -tC -tM -Od -v @&&|
  png16d.lib zlib.lib import32.lib  $(pngfix_OBJECTS) $(pngfix_EXTERNAL_OBJECTS)
|

# Rule to build all files generated by this target.
CMakeFiles\pngfix.dir\build: pngfix.exe
.PHONY : CMakeFiles\pngfix.dir\build

CMakeFiles\pngfix.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\pngfix.dir\cmake_clean.cmake
.PHONY : CMakeFiles\pngfix.dir\clean

CMakeFiles\pngfix.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "Borland Makefiles" C:\work\BAISim\3rdparty\libpng C:\work\BAISim\3rdparty\libpng C:\work\BAISim\3rdparty\libpng C:\work\BAISim\3rdparty\libpng C:\work\BAISim\3rdparty\libpng\CMakeFiles\pngfix.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\pngfix.dir\depend

