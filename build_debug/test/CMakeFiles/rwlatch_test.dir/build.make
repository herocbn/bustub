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
CMAKE_SOURCE_DIR = /home/cbn/Desktop/db/bustub

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cbn/Desktop/db/bustub/build_debug

# Include any dependencies generated for this target.
include test/CMakeFiles/rwlatch_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/rwlatch_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/rwlatch_test.dir/flags.make

test/CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.o: test/CMakeFiles/rwlatch_test.dir/flags.make
test/CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.o: ../test/common/rwlatch_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cbn/Desktop/db/bustub/build_debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.o"
	cd /home/cbn/Desktop/db/bustub/build_debug/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.o -c /home/cbn/Desktop/db/bustub/test/common/rwlatch_test.cpp

test/CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.i"
	cd /home/cbn/Desktop/db/bustub/build_debug/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cbn/Desktop/db/bustub/test/common/rwlatch_test.cpp > CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.i

test/CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.s"
	cd /home/cbn/Desktop/db/bustub/build_debug/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cbn/Desktop/db/bustub/test/common/rwlatch_test.cpp -o CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.s

# Object files for target rwlatch_test
rwlatch_test_OBJECTS = \
"CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.o"

# External object files for target rwlatch_test
rwlatch_test_EXTERNAL_OBJECTS =

test/rwlatch_test: test/CMakeFiles/rwlatch_test.dir/common/rwlatch_test.cpp.o
test/rwlatch_test: test/CMakeFiles/rwlatch_test.dir/build.make
test/rwlatch_test: lib/libbustub_shared.so
test/rwlatch_test: lib/libgmock_main.so.1.12.1
test/rwlatch_test: lib/libthirdparty_murmur3.so
test/rwlatch_test: lib/libgmock.so.1.12.1
test/rwlatch_test: lib/libgtest.so.1.12.1
test/rwlatch_test: test/CMakeFiles/rwlatch_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cbn/Desktop/db/bustub/build_debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable rwlatch_test"
	cd /home/cbn/Desktop/db/bustub/build_debug/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rwlatch_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/rwlatch_test.dir/build: test/rwlatch_test

.PHONY : test/CMakeFiles/rwlatch_test.dir/build

test/CMakeFiles/rwlatch_test.dir/clean:
	cd /home/cbn/Desktop/db/bustub/build_debug/test && $(CMAKE_COMMAND) -P CMakeFiles/rwlatch_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/rwlatch_test.dir/clean

test/CMakeFiles/rwlatch_test.dir/depend:
	cd /home/cbn/Desktop/db/bustub/build_debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cbn/Desktop/db/bustub /home/cbn/Desktop/db/bustub/test /home/cbn/Desktop/db/bustub/build_debug /home/cbn/Desktop/db/bustub/build_debug/test /home/cbn/Desktop/db/bustub/build_debug/test/CMakeFiles/rwlatch_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/rwlatch_test.dir/depend

