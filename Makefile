CC = gcc 
MAKE = make
TARGET=tick

#源文件
SRCS_DIRS += ./src/core/*.c
SRCS_DIRS += ./src/events/*.c
SRCS_DIRS += ./src/scheduling/*.c
SRCS_DIRS += ./src/os/unix/*.c
SRCS_DIRS += ./src/os/win32/*.c
SRCS_DIRS += ./src/module/*.c
SRCS_DIRS += ./src/module/akfs/*.c

#添加头文件
HEAD += -I$(shell pwd)/src/core
HEAD += -I$(shell pwd)/src/events
HEAD += -I$(shell pwd)/src/os
HEAD += -I$(shell pwd)/src/scheduling
HEAD += -I$(shell pwd)/src/module

#添加akfs模块
HEAD += -I$(shell pwd)/src/module/akfs

#编译参数
CFLAGS += $(HEAD)  

#编译选项
CFLAGS += -g -Wall 
CFLAGS += -pipe  -O0 -Wall -g3 -ggdb3
CFLAGS += -Wunused-variable

CFLAGS += -D_XOPEN_SOURCE=600

#编译操作系统制定
CFLAGS += -D__USE_UNIX__

#内存溢出检测
#CFLAGS += -fsanitize=address -fno-omit-frame-pointer

#动态库
LDFLAGS += -ldl -lrt -lpthread -lm -rdynamic

#静态库
LIBS +=

#编译目录
objs_dir=objs
vpath %.c src
source = $(wildcard *.c $(SRCS_DIRS) $(notdir $(wildcard src/*.c)))
objs = $(source:%.c=$(objs_dir)/%.o)

.PHONY: all clean

all: $(TARGET)  
$(TARGET) : $(objs) 
	@echo -e "\033[32mGenerate the ELF file: $(TARGET) \033[0m"
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)
$(objs): $(objs_dir)/%.o : %.c
	@echo -e "\033[32mCompile the object file: $<\033[0m"
	$(CC) $(CFLAGS) -o $@ -c $<
clean:
	@echo -e "\033[32mDelete compiled file \033[0m"
	rm -rf $(foreach i,$(shell find ./objs -name "*.o") ,$(i)) $(TARGET) 
