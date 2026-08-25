CC ?= gcc
CFLAGS ?= -std=c99 -O2 -Wall

SDK_ROOT = ../..
SDK_INCLUDE = $(SDK_ROOT)/include
SDK_LIB = $(SDK_ROOT)/lib/linux-x86_64/libblstsdk.a

PNG_LDFLAGS ?= $(shell pkg-config --libs libpng)
PNG_CPPFLAGS ?= $(shell pkg-config --cflags libpng)

TARGETS = thermal_stats thermal_stats_blst

all: $(TARGETS)

thermal_stats: thermal_stats.c
	$(CC) $(CFLAGS) $(PNG_CPPFLAGS) $< -o $@ $(PNG_LDFLAGS)

thermal_stats_blst: thermal_stats_blst.c $(SDK_LIB)
	$(CC) $(CFLAGS) -I$(SDK_INCLUDE) $< $(SDK_LIB) -o $@ -lm

clean:
	rm -f $(TARGETS)

.PHONY: all clean
