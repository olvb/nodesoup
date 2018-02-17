DEBUG = 0

PACKAGE = nodesoup
LIB_TARGET = lib/$(PACKAGE)/lib$(PACKAGE).so
DEMO_TARGETS = bin/dot2png

CXX = g++
LD = $(CXX)
CXXFLAGS = -std=c++14 -Wall -Wextra -Wno-sign-compare -Iinclude/$(PACKAGE)
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

HEADERS = $(wildcard include/*.hpp)
LIB_HEADERS = $(wildcard src/*.hpp)
LIB_SRCS = $(wildcard src/*.cpp)
LIB_OBJS = $(patsubst src/%.cpp, obj/%.o, $(LIB_SRCS))
DEMO_SRCS = $(wildcard demo/*.cpp)
DEPS = $(wildcard .d/*.d)

.PHONY: all lib demo clean

all: lib demo

lib: $(LIB_TARGET)

demo: $(DEMO_TARGETS)

$(LIB_TARGET): $(LIB_OBJS)
	@mkdir -p $(@D)
	$(LD) -shared $^ -o $@ $(LDFLAGS) $(LIB_LDFLAGS)

$(DEMO_TARGETS): $(LIB_OBJS)
	@mkdir -p $(@D) .d
	@echo $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $(DEMO_CXXFLAGS) -MMD -MF .d/$(@F).d -o $@ demo/$(@F).cpp $^ $(LDFLAGS) $(DEMO_LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

obj/%.o: src/%.cpp
	@mkdir -p $(@D) .d
	$(CXX) $(CXXFLAGS) $(LIB_CXXFLAGS) -MMD -MF .d/$*.d -c -o $@ $<

clean:
	$(RM) obj/* $(LIB_TARGET) $(DEMO_TARGETS) .d/*
