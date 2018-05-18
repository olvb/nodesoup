DEBUG = 0

PACKAGE = nodesoup
LIB_TARGET = lib/lib$(PACKAGE).so
DEMO_TARGETS = bin/dot2png

CXX = g++
LD = $(CXX)
CXXFLAGS = -std=c++14 -Wall -Wextra -Wno-sign-compare -Iinclude/
LDFLAGS =

ifeq ($(DEBUG), 1)
CXXFLAGS += -g -DDEBUG
else
CXXFLAGS += -O2 -DNDEBUG
endif

LIB_CXXFLAGS = -fPIC
LIB_LDFLAGS = -lm

DEMO_CXXFLAGS = $(shell pkg-config --cflags cairo)
DEMO_LDFLAGS = $(shell pkg-config --libs cairo)

LIB_SRCS = $(wildcard src/*.cpp)
LIB_OBJS = $(patsubst src/%.cpp, obj/lib/%.o, $(LIB_SRCS))
LIB_DEPS = $(wildcard .d/lib/*.d)
DEMO_SRCS = $(wildcard demo/*.cpp)
DEMO_DEPS = $(wildcard .d/demo/*.d)

.PHONY: all lib demo clean

all: lib demo

lib: $(LIB_TARGET)

demo: $(DEMO_TARGETS)

lib/lib$(PACKAGE).so: $(LIB_OBJS)
	@mkdir -p $(@D)
	$(LD) -shared $^ -o $@ $(LDFLAGS) $(LIB_LDFLAGS)

bin/%: obj/demo/%.o $(LIB_OBJS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS) $(DEMO_LDFLAGS)

obj/lib/%.o: src/%.cpp
	@mkdir -p $(@D) .d/lib
	$(CXX) $(CXXFLAGS) $(LIB_CXXFLAGS) -MMD -MF .d/lib/$*.d -c -o $@ $<

obj/demo/%.o: demo/%.cpp $(LIB_OBJS)
	@mkdir -p $(@D) .d/demo
	$(CXX) $(CXXFLAGS) $(DEMO_CXXFLAGS) -MMD -MF .d/demo/$*.d -c -o $@ $<

ifneq ($(MAKECMDGOALS), clean)
-include $(LIB_DEPS)
-include $(DEMO_DEPS)
endif

clean:
	$(RM) lib/* bin/* obj/lib/* obj/demo/* .d/lib/* .d/demo/*
