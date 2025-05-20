

# Define the compiler
CC = clang-19
LLC = llc
LD = wasm-ld

# Define compiler flags (e.g., -Wall for warnings, -g for debugging)
CFLAGS = --target=wasm32 -mbulk-memory -emit-llvm -c -S -I. -I../include -I/usr/include/
LLCFLAGS = -march=wasm32 -filetype=obj
LDFLAGS = --no-entry --export-all

# Define the source files (use a wildcard to include all .c files)
SRCS = $(wildcard *.c) ../src/drawing.c ../src/canvas.c
LLS = $(patsubst %.c,%.ll,$(SRCS))
OBJS = $(patsubst %.c,%.o,$(SRCS))


# Define the object files (derived from source files)
# OBJS = $(SRCS:%.c=%.o)
# WASMS = $(SRCS:%.c=%.wasm)

all: drinks.wasm

%.o: %.c
	$(CC) $(CFLAGS) -o $(patsubst %.c,%.ll,$@) $<
	$(LLC) $(LLCFLAGS) -o $@ $(patsubst %.c,%.ll,$@)

# Target for running the llc command
# %.o: %.ll

drinks.wasm: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	cp $@ ../public

# Target for cleaning up
clean:
	rm -f drinks.o drinks.wasm $(LLS) $(OBJS)

	



# greenland2:
# 	clang --target=wasm32 -mbulk-memory -emit-llvm -c -S -I../include -I/usr/include/ -o greenland.ll greenland2.c
# 	llc -march=wasm32 -filetype=obj -o greenland.o greenland.ll 
# 	wasm-ld --no-entry --export-all -o greenland.wasm greenland.o
# 	cp greenland.wasm ../public