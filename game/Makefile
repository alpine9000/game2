GAME_NAME=game2

# userstack used by bootblock
USERSTACK_ADDRESS=7fffc
# if you change this you must also change link.script.x
BASE_ADDRESS=4000
# note: this must be high enough not to conflict with MFMbufE
BOOTBLOCK_ASM=alpine_bootblock.s
SRCS=hw_asm.s audio.c glcdfont.c si.c gfx.c trackloader.s init.c screen.c
P61_MUSIC=0

IMAGES=sprite.png
IMAGEDATA=$(addprefix out/, $(IMAGES:.png=.bin))
ASSETS=$(IMAGEDATA) out/invaderkilled.raw out/explosion.raw out/shoot.raw out/beat1.raw out/beat2.raw out/beat3.raw out/beat4.raw

MAX_P61_SIZE=$(shell ls -Sl assets/P61.* | awk '{print $$5}' | head -1)

LINKER_OPTIONS=-T link.script.x
VASM_ARGS=-Fhunk -quiet -esc -DSFX=1 -allmp -DP61_MUSIC=$(P61_MUSIC) -DMAX_P61_SIZE=$(MAX_P61_SIZE)
AMIGA_INCLUDE=/usr/local/amiga/m68k-amigaos/ndk/include
C_LIBS=/usr/local/amiga/m68k-amigaos/vbcc/lib/vcs.lib

ADF=bin/$(GAME_NAME).adf
MODULE=$(GAME_NAME).s
PROGRAM_BIN=out/main.bin

VASM=/usr/local/amiga/m68k-amigaos/bin/vasmm68k_mot
CFLAGS=-warn=-1 -dontwarn=170 -dontwarn=306 -dontwarn=166 -dontwarn=167  -dontwarn=307 -dontwarn=208 -maxoptpasses=100  -c99 -O3

ASM_SRCS=$(SRCS:.s=.o)
C_SRCS=$(ASM_SRCS:.c=.o)
ADF_OBJS=$(addprefix out/adf/, $(C_SRCS))
WORKBENCH_OBJS=$(addprefix out/workbench/, $(C_SRCS:.s=.o))
ALL_TOOLS=$(MAKEADF) $(IMAGECON) $(MAPGEN) $(FADE) $(CROPPA) $(RESIZE)
ALL_OBJS=out/adf/main.o $(ADF_OBJS)
ALL_WORKBENCH_OBJS=out/workbench/main.o $(WORKBENCH_OBJS)
ALL_DEPENDS=$(ALL_OBJS:.o=.d) $(ALL_WORKBENCH_OBJS:.o=.d)


adf: bin out $(ALL_TOOLS) $(ADF)

all: adf 

gdrive: all
	cp $(ADF) ~/Google\ Drive

test: all
	cp $(ADF) ~/Projects/amiga/test.adf

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

$(ADF): out/bootblock.bin
	$(MAKEADF) out/bootblock.bin > $(ADF)
	@ls -lh out/bootblock.bin

wb: bin/$(GAME_NAME)

out/sprite.bin: assets/sprite.png
	$(IMAGECON) --quantize --colors=2 --full-color-palette --use-palette=palette.pal --input=assets/sprite.png --output=out/sprite --output-bitplanes --output-png --output-palette

out/invaderkilled.raw: assets/invaderkilled.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@	
out/explosion.raw: assets/explosion.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@	
out/shoot.raw: assets/shoot.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@	
out/beat1.raw: assets/fastinvader1.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@
out/beat2.raw: assets/fastinvader2.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@
out/beat3.raw: assets/fastinvader3.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@
out/beat4.raw: assets/fastinvader4.wav
	sox -v 0.05 $<  -b 8 -c 1 -r 11025 $@

out/bootblock.bin: out/bootblock.o
	vlink -brawbin1 $< -o $@

out/bootblock.o: $(BOOTBLOCK_ASM) $(PROGRAM_BIN)
	$(VASM) $(VASM_ARGS)  -DUSERSTACK_ADDRESS="\$$$(USERSTACK_ADDRESS)" -DBASE_ADDRESS="\$$$(BASE_ADDRESS)"  $< -o $@ -I$(AMIGA_INCLUDE)

out/adf/main.o: $(MODULE) $(EXTRA) $(ASSETS) $(IMAGEDATA)
	$(VASM)  -DTRACKLOADER=1  $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE)
	@$(VASM) -depend=make -DTRACKLOADER=1 $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE) > $*.d

out/workbench/main.o: $(MODULE) $(EXTRA) $(ASSETS)
	$(VASM)  -DTRACKLOADER=0 $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE)
	@$(VASM) -depend=make -DTRACKLOADER=0 $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE) > $*.d

out/adf/%.o: %.s
	$(VASM) $(VASM_ARGS)  -DTRACKLOADER=1 $< -o $@ -I$(AMIGA_INCLUDE)
	@$(VASM) -depend=make -DTRACKLOADER=1 $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE) > out/adf/$*.d

out/adf/%.o: %.c
	-@vc -deps $(CFLAGS) -S -DTRACKLOADER=1 -I$(AMIGA_INCLUDE) $< -o out/adf/$*.s > /dev/null 2> /dev/null
	-@/bin/echo -n "out/adf/" | cat - $*.dep > out/adf/$*.d
	-@rm $*.dep
	vc $(CFLAGS) -DTRACKLOADER=1 -I$(AMIGA_INCLUDE) -c $< -o $@

out/workbench/%.o: %.c
	-@vc -deps $(CFLAGS) -S -DTRACKLOADER=0 -I$(AMIGA_INCLUDE) $< -o out/workbench/$*.s > /dev/null 2> /dev/null
	-@/bin/echo -n "out/workbench/" | cat - $*.dep > out/workbench/$*.d
	-@rm $*.dep
	vc $(CFLAGS) -DTRACKLOADER=0 -I$(AMIGA_INCLUDE) -c $< -o $@

out/workbench/%.o: %.s
	$(VASM)  -DTRACKLOADER=0 $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE)
	@$(VASM) -depend=make  -DTRACKLOADER=1 $(VASM_ARGS) $< -o $@ -I$(AMIGA_INCLUDE) > out/workbench/$*.d


out/main.bin: out/adf/main.o $(ADF_OBJS)
	vlink $(LINKER_OPTIONS)  -brawbin1 $< $(ADF_OBJS) $(C_LIBS) -o $@ 
	@vlink $(LINKER_OPTIONS) -brawbin1 $< $(ADF_OBJS) $(C_LIBS) -M -o /tmp/main.bin | grep ", value " | cut -d " " -f3,7 | cut -d "," -f1 > $@.symbols
	@cp $@.symbols ~/Projects/amiga/debugger.syms
	@echo "RAM USAGE:" $$((16#`cat ~/Projects/amiga/debugger.syms | grep endRam | sed 's/0x//' | sed 's/endRam: //'`)) bytes

bin/$(GAME_NAME): bin out $(ALL_TOOLS)  $(ALL_WORKBENCH_OBJS) 
	vlink $(C_LIBS)	-hunkattr DATA=2 -hunkattr CODE=2 -hunkattr .text=2 -hunkattr .code=2 -hunkattr .data=2	-hunkattr .noload=2 -hunkattr .bss=2 $(ALL_WORKBENCH_OBJS) -o $@
	cp $@ ~/A1200/BlockySkies
	cp $@ ~/A4000/BlockySkies
	#cp assets/BlockySkies.info bin
	#cp assets/BlockySkiesDrawer.info bin
	#cp assets/BlockySkies.info ~/A1200
	#cp assets/BlockySkies.info ~/A4000
	#ls -lh bin/BlockySkies

.PHONY: clean
clean:	
	rm -rf out bin *~

depend:
	echo $(ALL_DEPENDS)

-include $(ALL_DEPENDS)
$(ALL_OBJS): Makefile
$(ALL_WORKBENCH_OBJS): Makefile

include externals.mk
include run.mk