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
	$(wildcard ../ugui/fonts/*.c) \
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

