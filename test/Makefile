# Build der Tests. Pfade wie im Server-Makefile ueberschreibbar:
#   make IPAMIR_DIR=/pfad/zum/solver CPLEX_LIB_DIR=/pfad/zu/cplex/lib

CXX      ?= g++

IPAMIR_DIR     ?= /opt/incremental-maxhs/src/build/release
IPAMIR_INCLUDE ?= $(IPAMIR_DIR)/ipamir
IPAMIR_LIB_DIR ?= $(IPAMIR_DIR)/lib
IPAMIR_LIB     ?= ipamirmaxhs
CPLEX_LIB_DIR  ?= /opt/cplex/cplex/lib/x86-64_linux/static_pic

CXXFLAGS ?= -std=c++11 -Wall -Wextra -I../src/server/ -I$(IPAMIR_INCLUDE)
LDFLAGS   = -L$(IPAMIR_LIB_DIR) -L$(CPLEX_LIB_DIR)
LDLIBS    = -l$(IPAMIR_LIB) -lcplex -lz -lpthread -ldl

TARGET = test_ipamir_solver
SRCS   = test_ipamir_solver.cc

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) $(LDLIBS) -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o
