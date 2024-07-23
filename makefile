DST_DIR   = ./build
$(shell mkdir -p $(DST_DIR))

ifeq ($(wildcard ./build/test.pas), )
    $(shell cp ./test.pas $(DST_DIR))
endif

CXX = g++
SRCS   := $(wildcard ./*.cpp ./src/*.cpp ./*.cc ./src/*.cc)
DEPS   := $(wildcard ./include/*.h) $(SRCS)

# SRCS   := $(shell find . -maxdepth 2 -name "*.c")
# DEPS   := $(shell find . -maxdepth 2 -name "*.h") $(SRCS)

INC_PATH = ./include
# -I 表示添加头文件目录
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS   = -Wall -Werror $(INCFLAGS)
TARGET_NAME = compiler
TARGET   = $(DST_DIR)/$(TARGET_NAME)


all: $(TARGET)

$(TARGET): $(DEPS)
	$(CXX) $(CFLAGS) -o $@ $(SRCS)
# $(CXX) $(CFLAGS) -o $@ $(realpath $(SRCS))

run: $(TARGET)
	cd build && ./$(TARGET_NAME)

gdb: $(DEPS)
	$(CXX) -g $(CFLAGS) -o $(TARGET) $(SRCS)
	gdb $(TARGET)

.PHONY:clean
clean:
	cd build && rm -f test.dyd test.err test.pro test.var $(TARGET_NAME) test.dys