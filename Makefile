# Name: Nicholas Andujar
# Email Handle: na18318@ohio.edu

#Variables
#CXX = g++


# The .PHONY target mark the list of targets
# as none-file.
.PHONY: all build archive clean

#TARGET=

build: map_main

map_main:
	$(CXX) -Wall map.cc
