DEBUG = 0

CXX = g++
LD = $(CXX)
CXXFLAGS = -std=c++14 -Wall -Wextra -Wno-sign-compare
CXXFLAGS += `pkg-config --cflags cairo`
LDFLAGS = -lm
LDFLAGS += `pkg-config --libs cairo`

CXXFLAGS_DBG = -g -DDEBUG
CXXFLAGS_RLS = -O2 -DNDEBUG
ifeq ($(DEBUG), 1)
	CXXFLAGS += $(CXXFLAGS_DBG)
else
	CXXFLAGS += $(CXXFLAGS_RLS)
endif

TARGET = nodesoup

SRCDIR = src
INCLUDEDIR = include
OBJDIR = obj
DEPDIR = .d

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.cpp, $(DEPDIR)/%.d, $(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o:
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MM -MT $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $<) $< -MF $@

ifneq ($(MAKECMDGOALS), clean)
    -include $(DEPS)
endif

clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET) .deps
