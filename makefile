TARGET = test

LDFLAGS = -march=x86-64

INCLUDES =

LIBS =  -lpthread -ldl -lrt

CXXFLAGS = -g -Wall $(LDFLAGS)

CXX := g++
CC  := gcc
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %cpp,%o,$(SRCS)) 
OBJS += md5.o

all: $(TARGET)

# build
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

md5.o: md5.c
	$(CC) $(CXXFLAGS) $(INCLUDES) -c md5.c

# link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -rf $(TARGET) *.o
