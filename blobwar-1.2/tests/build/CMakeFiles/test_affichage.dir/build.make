# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests/build"

# Utility rule file for test_affichage.

# Include any custom commands dependencies for this target.
include CMakeFiles/test_affichage.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_affichage.dir/progress.make

CMakeFiles/test_affichage: test_display
	./test_display

test_affichage: CMakeFiles/test_affichage
test_affichage: CMakeFiles/test_affichage.dir/build.make
.PHONY : test_affichage

# Rule to build all files generated by this target.
CMakeFiles/test_affichage.dir/build: test_affichage
.PHONY : CMakeFiles/test_affichage.dir/build

CMakeFiles/test_affichage.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_affichage.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_affichage.dir/clean

CMakeFiles/test_affichage.dir/depend:
	cd "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests" "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests" "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests/build" "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests/build" "/home/hugo/2A/Algo Projet/Projet-Algo/blobwar-1.2/tests/build/CMakeFiles/test_affichage.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/test_affichage.dir/depend

