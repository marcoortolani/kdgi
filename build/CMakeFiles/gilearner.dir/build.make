# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.10.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.10.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/andrea/Desktop/tesi_ort/code/kdgi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/andrea/Desktop/tesi_ort/code/kdgi/build

# Include any dependencies generated for this target.
include CMakeFiles/gilearner.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/gilearner.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/gilearner.dir/flags.make

CMakeFiles/gilearner.dir/main.cc.o: CMakeFiles/gilearner.dir/flags.make
CMakeFiles/gilearner.dir/main.cc.o: ../main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/andrea/Desktop/tesi_ort/code/kdgi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/gilearner.dir/main.cc.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gilearner.dir/main.cc.o -c /Users/andrea/Desktop/tesi_ort/code/kdgi/main.cc

CMakeFiles/gilearner.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gilearner.dir/main.cc.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/andrea/Desktop/tesi_ort/code/kdgi/main.cc > CMakeFiles/gilearner.dir/main.cc.i

CMakeFiles/gilearner.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gilearner.dir/main.cc.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/andrea/Desktop/tesi_ort/code/kdgi/main.cc -o CMakeFiles/gilearner.dir/main.cc.s

CMakeFiles/gilearner.dir/main.cc.o.requires:

.PHONY : CMakeFiles/gilearner.dir/main.cc.o.requires

CMakeFiles/gilearner.dir/main.cc.o.provides: CMakeFiles/gilearner.dir/main.cc.o.requires
	$(MAKE) -f CMakeFiles/gilearner.dir/build.make CMakeFiles/gilearner.dir/main.cc.o.provides.build
.PHONY : CMakeFiles/gilearner.dir/main.cc.o.provides

CMakeFiles/gilearner.dir/main.cc.o.provides.build: CMakeFiles/gilearner.dir/main.cc.o


# Object files for target gilearner
gilearner_OBJECTS = \
"CMakeFiles/gilearner.dir/main.cc.o"

# External object files for target gilearner
gilearner_EXTERNAL_OBJECTS =

gilearner: CMakeFiles/gilearner.dir/main.cc.o
gilearner: CMakeFiles/gilearner.dir/build.make
gilearner: gi/libgi_lib.a
gilearner: CMakeFiles/gilearner.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/andrea/Desktop/tesi_ort/code/kdgi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable gilearner"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gilearner.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/gilearner.dir/build: gilearner

.PHONY : CMakeFiles/gilearner.dir/build

CMakeFiles/gilearner.dir/requires: CMakeFiles/gilearner.dir/main.cc.o.requires

.PHONY : CMakeFiles/gilearner.dir/requires

CMakeFiles/gilearner.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/gilearner.dir/cmake_clean.cmake
.PHONY : CMakeFiles/gilearner.dir/clean

CMakeFiles/gilearner.dir/depend:
	cd /Users/andrea/Desktop/tesi_ort/code/kdgi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/andrea/Desktop/tesi_ort/code/kdgi /Users/andrea/Desktop/tesi_ort/code/kdgi /Users/andrea/Desktop/tesi_ort/code/kdgi/build /Users/andrea/Desktop/tesi_ort/code/kdgi/build /Users/andrea/Desktop/tesi_ort/code/kdgi/build/CMakeFiles/gilearner.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/gilearner.dir/depend

