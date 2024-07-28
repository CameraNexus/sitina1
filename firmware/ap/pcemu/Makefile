TARGET := emu
OBJODIR := obj

CC = gcc
LD = gcc

LIBS := $(shell pkg-config sdl2 --cflags --libs) -lm

COMMONFLAGS := \
	-Wall -Og -g $(shell pkg-config sdl2 --cflags)

LDFLAGS :=

INCLUDES += \
    -I.\
    -I../portable

CSRCS += \
	$(wildcard ./*.c) \
	$(wildcard ../portable/fonts/*.c) \
	$(wildcard ../portable/assets/*.c) \
	$(wildcard ../portable/*.c)

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
