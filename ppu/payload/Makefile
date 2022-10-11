#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------

.SUFFIXES:

#---------------------------------------------------------------------------------

ifeq ($(strip $(PLFW)),)
	$(error "Please set PLFW in your environment: export PLFW=<path>")
endif

include $(PLFW)/rules/ppu

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------

TARGET   := $(notdir $(CURDIR))
BUILD    := build
SOURCES  := source
DATA     := data
INCLUDES := include
LIB      := 
LDSCRIPT = ldscript.ld

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

ASFLAGS  = $(MACHDEP) -g -Os -Wall $(INCLUDE)
CFLAGS   = $(MACHDEP) -ffunction-sections -fdata-sections -nostdinc -fno-builtin -mno-toc -std=c99 -g -Os -Wall $(INCLUDE)
CXXFLAGS = $(MACHDEP) -ffunction-sections -fdata-sections -nostdinc -fno-builtin -mno-toc -g -Os -Wall $(INCLUDE)
LDFLAGS  = $(MACHDEP) -ffunction-sections -fdata-sections -nostartfiles -nodefaultlibs -Wl,--gc-sections -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------

LIBS := -ltier

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------

LIBDIRS := $(CURDIR)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

#---------------------------------------------------------------------------------

LDFLAGS += -T$(CURDIR)/$(LDSCRIPT)

#---------------------------------------------------------------------------------

export OUTPUT := $(CURDIR)/$(TARGET)

export FINAL := $(CURDIR)/$(addsuffix .c,$(TARGET))

export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                $(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR := $(CURDIR)/$(BUILD)

export BUILDDIR := $(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------

CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cc)))
sFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------

ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif

export OFILES := $(addsuffix .o,$(BINFILES)) \
                 $(CXXFILES:.cc=.cc.o) $(CFILES:.c=.c.o) \
                 $(sFILES:.s=.s.o) $(SFILES:.S=.S.o)

#---------------------------------------------------------------------------------
# build paths
#---------------------------------------------------------------------------------

export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(CURDIR)/$(BUILD) \
                  -I$(PLFW)/tier/include

export LIBPATHS	:= $(foreach dir,$(LIB),-L$(CURDIR)/$(dir)) \
                   $(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
                  -L$(PLFW)/tier

#---------------------------------------------------------------------------------

.PHONY: $(BUILD) clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(FINAL): $(OUTPUT).pl $(BUILD)
	@$(bin2c)

#---------------------------------------------------------------------------------

final: $(FINAL)
	@$(ECHO) making source for binary payload ... $(notdir $(FINAL))

clean:
	@$(ECHO) clean ...
	$(RM) -r $(BUILD) $(OUTPUT).elf $(OUTPUT).elf.map $(OUTPUT).pl $(FINAL)

#---------------------------------------------------------------------------------

else

#---------------------------------------------------------------------------------

LDFLAGS += -T$(CURDIR)/../$(LDSCRIPT)

#---------------------------------------------------------------------------------

DEPENDS := $(addsuffix .d,$(BINFILES)) \
           $(CXXFILES:.cc=.cc.d) $(CFILES:.c=.c.d) \
           $(sFILES:.s=.s.d) $(SFILES:.S=.S.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------

%.bin.o: %.bin
	@$(ECHO) $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------

-include $(DEPENDS)

#---------------------------------------------------------------------------------

endif
