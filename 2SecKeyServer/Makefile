SRC:=$(wildcard ./*.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(SRC))
GCC:=g++
INC:=./

TARGET:=server

all:$(TARGET)
$(TARGET):$(OBJS)
	$(GCC) $^ -I$(INC) -lssl -lcrypto -pthread  -o $@

%.o:%.cpp
	$(GCC) -g -c -std=c++11 $< -o $@

.PHONY:clean
clean:
	$(RM) $(OBJS) $(TARGET)
