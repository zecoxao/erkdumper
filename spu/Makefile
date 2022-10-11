CC=spu-gcc
STRIP=spu-strip
OBJCOPY=spu-objcopy

TARGET=dumper.elf
INC=-I.
LIB=

CFLAGS=-ffunction-sections -fdata-sections -fno-builtin -Os -nostdinc $(INC)
LDFLAGS=-nostartfiles -nostdlib -nodefaultlibs -Tldscript.ld -Wl,--gc-sections $(LIB)

SRC_ASM=start.s
SRC_C=main.c

OBJ=$(SRC_ASM:.s=.o) $(SRC_C:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $^

%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f $(TARGET) $(OBJ)
