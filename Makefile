DEBUG = 0

PACKAGE = nodesoup

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
LIB_LDFLAGS = -lm -fPIC

DEMO_CXXFLAGS = $(shell pkg-config --cflags cairo)
DEMO_LDFLAGS = $(shell pkg-config --libs cairo) -Llib -lnodesoup

HEADERS = $(wildcard include/*.hpp)
LIB_HEADERS = $(wildcard src/*.hpp)
LIB_SRCS = $(wildcard src/*.cpp)
LIB_OBJS = $(patsubst src/%.cpp, obj/%.o, $(LIB_SRCS))
LIB_TARGET = lib/lib$(PACKAGE).so
DEMO_SRCS = $(wildcard demo/*.cpp)
DEMO_TARGETS = $(patsubst demo/%.cpp, bin/%, $(DEMO_SRCS))
DEPS = $(wildcard .d/*.d)

.PHONY: all clean depend

all: $(LIB_TARGET) $(DEMO_TARGETS)

$(LIB_TARGET): $(LIB_OBJS)
	@mkdir -p $(@D) .d
	$(LD) -shared -MMD -MF .d/$(PACKAGE).d $^ -o $@ $(LDFLAGS) $(LIB_LDFLAGS)

$(DEMO_TARGETS): $(LIB_TARGET)
	@mkdir -p $(@D) .d
	$(CXX) $(CXXFLAGS) $(DEMO_CXXFLAGS) -MMD -MF .d/$(@F).d -o $@ $(DEMO_SRCS) $(LDFLAGS) $(DEMO_LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

obj/%.o: src/%.cpp
	@mkdir -p $(@D) .d
	$(CXX) $(CXXFLAGS) $(LIB_CXXFLAGS) -MMD -MF .d/$*.d -c -o $@ $<

clean:
	$(RM) $(LIB_OBJS) $(LIB_TARGET) $(DEMO_TARGETS) $(DEPS)
