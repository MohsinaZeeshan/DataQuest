# DataQuest Makefile (g++ on Linux, mingw32-make on Windows)
#   make         build the game  (needs src/main.cpp from Module C)
#   make run     build and launch the game
#   make test    build and run every tests/test_*.cpp
#   make clean   remove build output

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude

ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM  = del /Q
    FIX = $(subst /,\,$1)
else
    EXE =
    RM  = rm -f
    FIX = $1
endif

TARGET    = dataquest$(EXE)
ALL_SRC   = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
LIB_SRC   = $(filter-out src/main.cpp,$(ALL_SRC))
HEADERS   = $(wildcard include/*.h) $(wildcard include/*/*.h)
TEST_SRC  = $(wildcard tests/test_*.cpp)
TEST_BINS = $(patsubst tests/%.cpp,tests/%$(EXE),$(TEST_SRC))

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(ALL_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(ALL_SRC) -o $@

run: $(TARGET)
	"./$(TARGET)"

# Each test file is its own program, linked with every module's code except main.cpp
tests/%$(EXE): tests/%.cpp $(LIB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $< $(LIB_SRC) -o $@

test: $(TEST_BINS)
	$(foreach t,$(TEST_BINS),"./$(t)" &&) echo All tests finished

clean:
	-$(RM) $(call FIX,$(TARGET) $(TEST_BINS))
