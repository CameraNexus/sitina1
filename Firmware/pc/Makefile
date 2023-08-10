TARGET := emu
OBJODIR := obj/pc
RTLDIR := .

CC = gcc
LD = gcc

LIBS := $(shell pkg-config sdl2 --cflags --libs) -lm

COMMONFLAGS := \
	-Wall -Og -g -DLV_CONF_INCLUDE_SIMPLE $(shell pkg-config sdl2 --cflags)

LDFLAGS :=

INCLUDES += \
	-I.\
	-I../app \
	-I../ugui

CSRCS += \
	./main.c \
	./os_camera.c \
	./os_display.c \
	./os_filesystem.c \
	./os_input.c \
	./os_timer.c \
	../ugui/fonts/arial_10X13.c \
	../ugui/fonts/arial_10X13_cyrillic.c \
	../ugui/fonts/arial_12X15.c \
	../ugui/fonts/arial_12X15_cyrillic.c \
	../ugui/fonts/arial_16X18.c \
	../ugui/fonts/arial_16X18_cyrillic.c \
	../ugui/fonts/arial_20X23.c \
	../ugui/fonts/arial_20X23_cyrillic.c \
	../ugui/fonts/arial_25X28.c \
	../ugui/fonts/arial_25X28_cyrillic.c \
	../ugui/fonts/arial_29X35.c \
	../ugui/fonts/arial_29X35_cyrillic.c \
	../ugui/fonts/arial_35X40.c \
	../ugui/fonts/arial_35X40_cyrillic.c \
	../ugui/fonts/arial_39X45.c \
	../ugui/fonts/arial_39X45_cyrillic.c \
	../ugui/fonts/arial_45X52.c \
	../ugui/fonts/arial_45X52_cyrillic.c \
	../ugui/fonts/arial_49X57.c \
	../ugui/fonts/arial_49X57_cyrillic.c \
	../ugui/fonts/arial_6X6.c \
	../ugui/fonts/arial_6X6_cyrillic.c \
	../ugui/fonts/arial_9X10.c \
	../ugui/fonts/arial_9X10_cyrillic.c \
	../ugui/fonts/font_10x16.c \
	../ugui/fonts/font_12x16.c \
	../ugui/fonts/font_12x20.c \
	../ugui/fonts/font_16x26.c \
	../ugui/fonts/font_22x36.c \
	../ugui/fonts/font_24x40.c \
	../ugui/fonts/font_32x53.c \
	../ugui/fonts/font_4x6.c \
	../ugui/fonts/font_5x12.c \
	../ugui/fonts/font_5x8.c \
	../ugui/fonts/font_6x10.c \
	../ugui/fonts/font_6x8.c \
	../ugui/fonts/font_7x12.c \
	../ugui/fonts/font_8x12.c \
	../ugui/fonts/font_8x12_cyrillic.c \
	../ugui/fonts/font_8x14.c \
	../ugui/fonts/font_8x8.c \
	../ugui/ugui_button.c \
	../ugui/ugui.c \
	../ugui/ugui_checkbox.c \
	../ugui/ugui_image.c \
	../ugui/ugui_progress.c \
	../ugui/ugui_textbox.c \
	../app/app_main.c \
	../app/gui.c \
	../app/image_processing.c

OBJS :=	$(CSRCS:%.c=$(OBJODIR)/%.o)

$(OBJODIR)/%.o: %.c $(RTLOBJ)
	@echo [CC] $<
	@mkdir -p $(dir $@)
	@$(CC) $(COMMONFLAGS) $(CCFLAGS) $(INCLUDES) -c -o $@ $<

PHONY += all
all: $(OBJS)
	@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)
	@echo 'all finish'

PHONY += clean
clean:
	rm -rf obj/
	rm -f $(TARGET)
	@echo 'clean finish'

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)

# Set default target
.DEFAULT_GOAL:= all

