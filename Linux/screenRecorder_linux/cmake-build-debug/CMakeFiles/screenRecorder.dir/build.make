# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/screenRecorder1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/screenRecorder1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/screenRecorder.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/screenRecorder.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/screenRecorder.dir/flags.make

CMakeFiles/screenRecorder.dir/main.c.o: CMakeFiles/screenRecorder.dir/flags.make
CMakeFiles/screenRecorder.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/screenRecorder1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/screenRecorder.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/screenRecorder.dir/main.c.o   -c /root/screenRecorder1/main.c

CMakeFiles/screenRecorder.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/screenRecorder.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/screenRecorder1/main.c > CMakeFiles/screenRecorder.dir/main.c.i

CMakeFiles/screenRecorder.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/screenRecorder.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/screenRecorder1/main.c -o CMakeFiles/screenRecorder.dir/main.c.s

CMakeFiles/screenRecorder.dir/captureScreen.c.o: CMakeFiles/screenRecorder.dir/flags.make
CMakeFiles/screenRecorder.dir/captureScreen.c.o: ../captureScreen.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/screenRecorder1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/screenRecorder.dir/captureScreen.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/screenRecorder.dir/captureScreen.c.o   -c /root/screenRecorder1/captureScreen.c

CMakeFiles/screenRecorder.dir/captureScreen.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/screenRecorder.dir/captureScreen.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/screenRecorder1/captureScreen.c > CMakeFiles/screenRecorder.dir/captureScreen.c.i

CMakeFiles/screenRecorder.dir/captureScreen.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/screenRecorder.dir/captureScreen.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/screenRecorder1/captureScreen.c -o CMakeFiles/screenRecorder.dir/captureScreen.c.s

# Object files for target screenRecorder
screenRecorder_OBJECTS = \
"CMakeFiles/screenRecorder.dir/main.c.o" \
"CMakeFiles/screenRecorder.dir/captureScreen.c.o"

# External object files for target screenRecorder
screenRecorder_EXTERNAL_OBJECTS =

screenRecorder: CMakeFiles/screenRecorder.dir/main.c.o
screenRecorder: CMakeFiles/screenRecorder.dir/captureScreen.c.o
screenRecorder: CMakeFiles/screenRecorder.dir/build.make
screenRecorder: CMakeFiles/screenRecorder.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/screenRecorder1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable screenRecorder"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/screenRecorder.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/screenRecorder.dir/build: screenRecorder

.PHONY : CMakeFiles/screenRecorder.dir/build

CMakeFiles/screenRecorder.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/screenRecorder.dir/cmake_clean.cmake
.PHONY : CMakeFiles/screenRecorder.dir/clean

CMakeFiles/screenRecorder.dir/depend:
	cd /root/screenRecorder1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/screenRecorder1 /root/screenRecorder1 /root/screenRecorder1/cmake-build-debug /root/screenRecorder1/cmake-build-debug /root/screenRecorder1/cmake-build-debug/CMakeFiles/screenRecorder.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/screenRecorder.dir/depend

