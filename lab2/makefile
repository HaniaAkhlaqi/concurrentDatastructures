CC := g++
CFLAGS := -Wall -g -lpthread
TARGET := a.out
FLAGS := ""

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard src/*.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(TARGET)

bench: CFLAGS += -O3
bench: clean
bench: all

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
obj/%.o: src/%.cpp src/*.hpp
	mkdir ./obj -p
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) obj/*.o
	
.PHONY: all clean
