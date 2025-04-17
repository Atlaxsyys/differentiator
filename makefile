.PHONY: all build run clean

OBJDIR = obj

all: build

build:
	make -C src build OBJDIR=$(abspath $(OBJDIR))

run:
	make -C src run OBJDIR=$(abspath $(OBJDIR))

clean:
	make -C src clean OBJDIR=$(abspath $(OBJDIR))