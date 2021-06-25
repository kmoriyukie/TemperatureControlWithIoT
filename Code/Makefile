all: printnodeid

# Linker optimizations
SMALL = 1

# Includes the project-conf configuration file
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"


# This flag includes the IPv6 libraries
CONTIKI_WITH_IPV6 = 1
CONTIKI = ../../../../contiki/
include $(CONTIKI)/Makefile.include
