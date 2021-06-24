DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

CONTIKI_PROJECT = main

RESOURCES_FILES = events_threads.c states.c

PROJECT_SOURCEFILES += $(RESOURCES_FILES)

all: $(CONTIKI_PROJECT)

APPS += mqtt

# Linker size optimization
SMALL = 1

CONTIKI_WITH_IPV6 = 1

CONTIKI = ../../../contiki

include $(CONTIKI)/Makefile.include
