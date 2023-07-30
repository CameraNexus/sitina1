TARGET := emu
OBJODIR := obj
RTLDIR := .

CC = gcc
LD = gcc

LIBS := $(shell pkg-config sdl2 --cflags --libs) -lm

COMMONFLAGS := \
	-Wall -Og -g -DLV_CONF_INCLUDE_SIMPLE $(shell pkg-config sdl2 --cflags)

LDFLAGS :=

# Include LVGL
LVGL_DIR_NAME := lvgl
LVGL_DIR := ..

include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

INCLUDES += \
	-I.\
	-I../app \
	-I../lvgl

CSRCS += \
	./main.c \
	./os_display.c \
	./os_filesystem.c \
	./os_input.c \
	./os_timer.c \
	../app/app_main.c \
	../app/lv_port.c

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
	rm -rf $(OBJODIR)
	rm -f $(TARGET)
	@echo 'clean finish'

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)

# Set default target
.DEFAULT_GOAL:= all

