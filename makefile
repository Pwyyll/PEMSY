# Praktikum: Eingebettete Mikrocontroller-Systeme
# (PEMSY)
# 
# Beispiel MAKEFILE. 
#
# Quelldateien werden im Unterverzeichnis src erwartet.
# Für jedes neue Programm wird ein entsprechendes
# Target benötigt.

# Der verwendete Controller
MCU := atmega32

# Unser Quellverzeichnis
SRCDIR		:= ./Kap_10_LCD

# Pfade zu den verschiedenen Programmen
CC		:= avr-gcc
OBJCOPY 	:= avr-objcopy
SIZE   		:= avr-size
AVARICE		:= avarice
GDB		:= avr-gdb
AVRDUDE		:= avrdude

# Verschiedene Parameter 
CFLAGS 		:= -g -Wall -Wstrict-prototypes -mmcu=$(MCU) -I. -fexec-charset=ISO-8859-15 -std=c99
LDFLAGS          = -g -Wl,-Map=$(@:.elf=.map),--cref
OBJCOPY_OPT 	:= -R .eeprom
JTAG_OPT	:= -B 500KHz -j /dev/ttyS0
# PEMSY
#AVRDUDE_OPT	:= -p m32 -P /dev/ttyS0 -c ponyser
# AVRDRAGON ISP
AVRDUDE_OPT    := -p m32 -P usb -c dragon_isp
# AVRDRAGON JTAG does not work
#AVRDUDE_OPT    := -p m32 -P usb -c dragon_jtag

# Debug-Optionen
DEB_HOST	:= localhost
DEB_PORT	:= 4242
DEB_INITFILE	:= .gdbcmds

# Dieses Target wird ausgeführt wenn hinter make
# kein Parameter angegeben wurde
notarget:
	@echo Keine Paramter. Stopp!

# Hier können die jeweiligen Targets erzeugt werden.
# Für jedes Programm muss dabei angegeben werden welche 
# .o-Dateien zu der ELF Datei gebunden werden sollen.
#
# Beispiel 1: 
# Ein Programm besteht aus der C Datei p1.c und soll
# in die Datei p1.elf bzw. dann später in die Datei p1.hex
# übersetzt werden. Es muss jetzt lediglich angegeben werden
# wie die .elf Datei erzeugt wird:
#
# 	p1.elf: $(SRCDIR)/p1.o
#
# Dabei wurde davon ausgegangen, dass sich die Quelldatei
# in dem Untervereichnis src befindet.
#
# Beispiel 2:
# Ein Programm besteht aus den C Dateien p1.c und p2.c.
# Das fertige Programm soll den Namen prg.hex tragen:
#
# 	prg.elf: $(SRCDIR)/p1.o $(SRCDIR)/p2.o
#
# Falls eine Zeile nicht für alle .o Dateien ausreichen sollte,
# kann mit dem "\" Zeichen auf der nächsten Zeile weitergeschrieben
# werden.
#
#
#
# HIER BITTE DIE EIGENEN TARGETS HINSCHREIBEN:
#
#p1.elf: $(SRCDIR)/p1.o
#program.elf: $(SRCDIR)/program.o
#prg.elf: $(SRCDIR)/p1.o $(SRCDIR)/p2.o

lcd.o: lcd.c
led.o: led.c
pwm.o: pwm.c
timer.o: timer.c
tastatur.o: tastatur.c
programm03.o: programm03.c


programm03.elf: lcd.o led.o pwm.o timer.o tastatur.o programm03.o
programm03.hex: programm03.elf


#
# ENDE DER EIGENEN TARGETS
#


# Dieses Target löscht alle Zwischendateien
clean:
	rm -f *.hex
	rm -f *.bin
	rm -f *.elf
	rm -f *.map
	rm -f *.o
	rm -f *.s

# Rule, um aus einer .c Datei eine .o Datei zu erzeugen
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule, um eine ELF Datei zu erzeugen
%.elf:
	$(CC) $(LDFLAGS) -mmcu=$(MCU) $^ -o $@
	chmod a-x $@

# Rule, um eine ELF Datei in das Intel HEX Format umzuwandeln
%.hex: %.elf
	$(OBJCOPY) -O ihex $(OBJCOPY_OPT) $< $@

# Rule, um eine ELF Datei in das binäre Format umzuwandeln
%.bin: %.elf
	$(OBJCOPY) -O binary $(OBJCOPY_OPT) $< $@
	chmod a-x $@

# Dieses Target startet alle benötigten Programme um den OCD-
# Debugger zu benutzen. 
%.debug: %.elf
	-killall $(AVARICE)
	$(AVARICE) $(JTAG_OPT) --detach $(DEB_HOST):$(DEB_PORT)
	echo "target remote $(DEB_HOST):$(DEB_PORT)" >$(DEB_INITFILE) 
#	$(GDB) -x $(DEB_INITFILE) $<
#	ddd --gdb $(GDB) --args -x $(DEB_INITFILE) $<
	ddd --gdb --debugger "$(GDB) -x $(DEB_INITFILE) $<"
	-rm -f $(DEB_INITFILE)

# Target programmiert die Datei in den Mikrocontroller
# mit Hilfe des JTAGICE von Atmel
%.prog: %.hex
	$(AVRDUDE) $(AVRDUDE_OPT) -U flash:w:$<:i
#	$(AVARICE) $(JTAG_OPT) -e -p -v -f $< 

