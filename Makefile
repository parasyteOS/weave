.PHONY: all

CFLAGS = -Iinclude $(shell $(PKG_CONFIG) --cflags libsepol)
LDFLAGS = $(shell $(PKG_CONFIG) --libs libsepol)

SRCS = $(shell find . -name '*.cpp')

OBJS = $(SRCS:%.cpp=%.o)

all: weave

weave: $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

install: weave
	install -m0755 -D weave $(PREFIX)/bin/weave
