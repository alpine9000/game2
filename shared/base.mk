HOST_WARNINGS=-pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
HOST_CFLAGS=$(HOST_WARNINGS)

MAKEADFDIR=../tools/makeadf/
MAKEADF=$(MAKEADFDIR)/out/makeadf
IMAGECONDIR=../tools/imagecon
IMAGECON=$(IMAGECONDIR)/out/imagecon
RESIZEDIR=../tools/resize
RESIZE=$(RESIZEDIR)/out/resize
CROPPADIR=../tools/croppa
CROPPA=$(CROPPADIR)/out/croppa
MAPGENDIR=../tools/mapgen
MAPGEN=$(MAPGENDIR)/out/mapgen
FADEDIR=../tools/fade
FADE=$(FADEDIR)/out/fade
A500_RUN_SCRIPT=~/Google\ Drive/Amiga/amiga500.sh
A600_RUN_SCRIPT=~/Google\ Drive/Amiga/amiga600.sh
A1200T_RUN_SCRIPT=~/Google\ Drive/Amiga/amiga1200t.sh

#VASM_ARGS=-phxass -Fhunk -quiet -spaces
VASM_ARGS=-Fhunk -quiet -esc 

ifndef FLOPPY
FLOPPY=bin/$(EXAMPLE_NAME).adf
endif

ifndef MODULE
MODULE=$(EXAMPLE_NAME).s
endif

ifndef SHRINKLER
SHRINKLER=0
endif

ifndef BASE_ADDRESS
BASE_ADDRESS=70000
endif

ifndef DECOMPRESS_ADDRESS
DECOMPRESS_ADDRESS=10000
endif

ifndef RUN_SCRIPT
RUN_SCRIPT=$(A500_RUN_SCRIPT)
endif


ifeq ($(SHRINKLER),1)
ifndef BOOTBLOCK_ASM
BOOTBLOCK_ASM=../shared/shrinkler_bootblock.s
endif
PROGRAM_BIN=out/shrunk.bin
VASM_EXTRA_BOOTBLOCK_ARGS=-DSHRINKLER=$(SHRINKLER) -DDECOMPRESS_ADDRESS="\$$$(DECOMPRESS_ADDRESS)"
else
ifndef BOOTBLOCK_ASM
BOOTBLOCK_ASM=../shared/bootblock.s
endif
PROGRAM_BIN=out/main.bin
VASM_EXTRA_BOOTBLOCK_ARGS=-DSHRINKLER=$(SHRINKLER)
endif

ifndef LINKER_OPTIONS
LINKER_OPTIONS=-Ttext 0x$(BASE_ADDRESS)
endif

ALL_TOOLS=$(MAKEADF) $(IMAGECON) $(MAPGEN) $(FADE) $(CROPPA) $(RESIZE)

adf: bin out $(ALL_TOOLS) $(FLOPPY)

all: adf 

gdrive: all
	cp $(FLOPPY) ~/Google\ Drive

test: all
	cp $(FLOPPY) ~/Projects/amiga/test.adf

go: test
	 $(RUN_SCRIPT)

goaga: test
	 $(A1200T_RUN_SCRIPT)

list:
	m68k-amigaos-objdump  -b binary --disassemble-all out/bootblock.bin -m m68k > out/bootblock.txt

bin:
	mkdir bin

out:
	mkdir out
	mkdir out/adf
	mkdir out/workbench

ic:
	make -C $(IMAGECONDIR)

$(IMAGECON):
	make -C $(IMAGECONDIR)

$(SHRINKLEREXE):
	make -C $(SHRINKLERDIR)

$(RESIZE):
	make -C $(RESIZEDIR)

$(CROPPA):
	make -C $(CROPPADIR)

$(MAPGEN):
	make -C $(MAPGENDIR)

$(FADE):
	make -C $(FADEDIR)

$(MAKEADF):
	make -C $(MAKEADFDIR)

$(DOYNAMITE68K):
	make -C $(DOYNAMITE68KDIR)

$(FLOPPY): out/bootblock.bin
	$(MAKEADF) out/bootblock.bin > $(FLOPPY)
	@ls -lh out/bootblock.bin

out/bootblock.bin: out/bootblock.o
	vlink -brawbin1 $< -o $@

out/bootblock.o: $(BOOTBLOCK_ASM) $(PROGRAM_BIN)
	vasmm68k_mot $(VASM_ARGS) $(VASM_EXTRA_BOOTBLOCK_ARGS) -DUSERSTACK_ADDRESS="\$$$(USERSTACK_ADDRESS)" -DBASE_ADDRESS="\$$$(BASE_ADDRESS)"  $< -o $@ -I/usr/local/amiga/os-include

out/adf/main.o: $(MODULE) $(EXTRA)
	vasmm68k_mot  -DTRACKLOADER=1  $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include
	@vasmm68k_mot -depend=make -DTRACKLOADER=1 $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include > $*.d

out/workbench/main.o: $(MODULE) $(EXTRA)
	vasmm68k_mot  -DTRACKLOADER=0 $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include
	@vasmm68k_mot -depend=make -DTRACKLOADER=0 $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include > $*.d

out/adf/%.o: %.s
	vasmm68k_mot $(VASM_ARGS)  -DTRACKLOADER=1 $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include
	@vasmm68k_mot -depend=make -DTRACKLOADER=1 $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include > out/adf/$*.d

out/workbench/%.o: %.s
	vasmm68k_mot  -DTRACKLOADER=0 $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include
	@vasmm68k_mot -depend=make  -DTRACKLOADER=1 $(VASM_ARGS) $(VASM_EXTRA_ARGS) $< -o $@ -I/usr/local/amiga/os-include > out/workbench/$*.d

out/%.o: %.c
	vc -O3 -c $< -o $@
	-@vc -O3 -S $< -o out/$*.s > /dev/null 2> /dev/null
	-@vc -O0 -S $< -o out/$*-noopt.s > /dev/null 2> /dev/null

ALL_OBJS=out/adf/main.o $(ADF_OBJS)
ALL_WORKBENCH_OBJS=out/workbench/main.o $(WORKBENCH_OBJS)
ALL_DEPENDS=$(ALL_OBJS:.o=.d) $(ALL_WORKBENCH_OBJS:.o=.d)

out/main.bin: out/adf/main.o $(ADF_OBJS)
	vlink $(LINKER_OPTIONS)  -brawbin1 $< $(ADF_OBJS) -o $@
	@vlink $(LINKER_OPTIONS) -brawbin1 $< $(ADF_OBJS) -M -o /tmp/main.bin | grep ", value " | cut -d " " -f3,7 | cut -d "," -f1 > $@.symbols
	@cp $@.symbols ~/Projects/amiga/debugger.syms
	@echo "RAM USAGE:" $$((16#`cat ~/Projects/amiga/debugger.syms | grep endRam | sed 's/0x//' | sed 's/endRam: //'`)) bytes


out/shrunk.bin: $(SHRINKLER_EXE) out/main.bin
	$(SHRINKLEREXE) -d out/main.bin out/shrunk.bin

.PHONE: clean
clean:
	rm -rf out bin *~

-include $(ALL_DEPENDS)