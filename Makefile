# Project directories:
dir_src := src
dir_inc := include
dir_obj := obj
dir_bin := bin

dirs_inc := $(dir_inc)

# Tools:
cc := gcc
ld := $(cc)

# Defines:
cdefines := _REENTRANT

# Libraries:
libs :=\
		SDL2

# Precompiler flags:
dflags := $(patsubst %,-D%,$(cdefines))
iflags := $(patsubst %,-I%,$(dirs_inc))

# Compiler flags:
cflags := -std=gnu17 -Wall -g3

# Linker flags:
lflags := $(patsubst %,-l%,$(libs))

# Object list:
obj_names :=\
		line-renderer\
		main

obj_files := $(patsubst %,$(dir_obj)/%.o,$(obj_names))

# Binary list:
bin := $(dir_bin)/bresenham

all: $(bin)

rebuild: clean $(bin)

run: $(bin)
	$(bin)

clean:
	rm -rf $(dir_obj) $(dir_bin)

# Directories
$(dir_obj):
	mkdir -p $@

$(dir_bin):
	mkdir -p $@

# Objects:
$(dir_obj)/%.o: $(dir_src)/%.c | $(dir_obj)
	$(cc) -c $(strip $(cflags) $(dflags) $(iflags)) -o $@ $<

# Binaries:
$(bin): $(obj_files) | $(dir_bin)
	$(ld) $(strip $(lflags)) -o $@ $^