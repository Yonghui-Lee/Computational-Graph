CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra -g

CXXFLAGS += -I src
CXXFLAGS += -MMD -MT $@

SRCS = $(wildcard src/*.cpp)
APPS = $(wildcard app/*.cpp)
SRC_OBJS = $(SRCS:src/%.cpp=build/src/%.o)
APP_BINS = $(APPS:app/%.cpp=%)

.PHONY: all
all: $(APP_BINS)

.PRECIOUS: build/src/%.o build/app/%.o

%: build/app/%.o $(SRC_OBJS)
	$(LINK.cc) $(OUTPUT_OPTION) $^

build/src/%.o: src/%.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

build/app/%.o: app/%.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

pack.zip:
	bsdtar --format zip -cf pack.zip --exclude 'build|$(APP_BINS)|$(APP_BINS:%=%.exe)' build app src doc.md README.md Makefile developer.txt

.PHONY: clean

clean:
	rm -rf build/*/*.o build/*/*.d $(APP_BINS) $(APP_BINS:%=%.exe)

-include $(SRC_OBJS:.o=.d)

-include $(APPS:app/%.cpp=build/app/%.d)
