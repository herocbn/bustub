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
CMAKE_SOURCE_DIR = /home/cbn/DB/bustub

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cbn/DB/bustub/standard

# Include any dependencies generated for this target.
include test/CMakeFiles/parallel_buffer_pool_manager_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/parallel_buffer_pool_manager_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/parallel_buffer_pool_manager_test.dir/flags.make

test/CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.o: test/CMakeFiles/parallel_buffer_pool_manager_test.dir/flags.make
test/CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.o: ../test/buffer/parallel_buffer_pool_manager_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cbn/DB/bustub/standard/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.o"
	cd /home/cbn/DB/bustub/standard/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.o -c /home/cbn/DB/bustub/test/buffer/parallel_buffer_pool_manager_test.cpp

test/CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.i"
	cd /home/cbn/DB/bustub/standard/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cbn/DB/bustub/test/buffer/parallel_buffer_pool_manager_test.cpp > CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.i

test/CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.s"
	cd /home/cbn/DB/bustub/standard/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cbn/DB/bustub/test/buffer/parallel_buffer_pool_manager_test.cpp -o CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.s

# Object files for target parallel_buffer_pool_manager_test
parallel_buffer_pool_manager_test_OBJECTS = \
"CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.o"

# External object files for target parallel_buffer_pool_manager_test
parallel_buffer_pool_manager_test_EXTERNAL_OBJECTS =

test/parallel_buffer_pool_manager_test: test/CMakeFiles/parallel_buffer_pool_manager_test.dir/buffer/parallel_buffer_pool_manager_test.cpp.o
test/parallel_buffer_pool_manager_test: test/CMakeFiles/parallel_buffer_pool_manager_test.dir/build.make
test/parallel_buffer_pool_manager_test: lib/libbustub_shared.so
test/parallel_buffer_pool_manager_test: lib/libgmock_main.so.1.12.1
test/parallel_buffer_pool_manager_test: lib/libthirdparty_murmur3.so
test/parallel_buffer_pool_manager_test: lib/libgmock.so.1.12.1
test/parallel_buffer_pool_manager_test: lib/libgtest.so.1.12.1
test/parallel_buffer_pool_manager_test: test/CMakeFiles/parallel_buffer_pool_manager_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cbn/DB/bustub/standard/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable parallel_buffer_pool_manager_test"
	cd /home/cbn/DB/bustub/standard/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/parallel_buffer_pool_manager_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/parallel_buffer_pool_manager_test.dir/build: test/parallel_buffer_pool_manager_test

.PHONY : test/CMakeFiles/parallel_buffer_pool_manager_test.dir/build

test/CMakeFiles/parallel_buffer_pool_manager_test.dir/clean:
	cd /home/cbn/DB/bustub/standard/test && $(CMAKE_COMMAND) -P CMakeFiles/parallel_buffer_pool_manager_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/parallel_buffer_pool_manager_test.dir/clean

test/CMakeFiles/parallel_buffer_pool_manager_test.dir/depend:
	cd /home/cbn/DB/bustub/standard && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cbn/DB/bustub /home/cbn/DB/bustub/test /home/cbn/DB/bustub/standard /home/cbn/DB/bustub/standard/test /home/cbn/DB/bustub/standard/test/CMakeFiles/parallel_buffer_pool_manager_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/parallel_buffer_pool_manager_test.dir/depend

