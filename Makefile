# Makefile for roach 2 hardware monitor

#SENSORLIB ?= ../lm-sensors/
#SENSORSLIB ?= /home/kat/mathews
KATCPLIB ?= /home/kat/mathews/katcp_devel/katcp/

INC = -I$(KATCPLIB)
LIB = -L$(KATCPLIB) -lkatcp

CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -DKATCP_USE_FLOATS
EXE = main
DBG_EXE = maind
SOURCE = main.c sensors.c card.c rw.c
ECHO = echo

SERVER = dbelab00

all: $(EXE)

main:
	$(CC) $(CFLAGS) -O2 -o $@ $(INC) $(SOURCE) $(LIB)

debug: 
	$(CC) $(CFLAGS) -ggdb -o $(DBG_EXE) $(INC) $(SOURCE) $(LIB)

clean: 
	$(RM) $(EXE)
	$(RM) $(DBG_EXE)
        
static: 
	$(CC) $(CFLAGS) -O2 -o $(EXE) $(INC) $(SOURCE) -static $(LIB) -lm

install: $(EXE)
	$(ECHO) "Attempting to copy $(EXE) to server..."
	scp $(EXE) $(USER)@$(SERVER):/home/nfs/test/.

