CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT) 

CONTIKI = ../../../../contiki

SMALL=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
PROJECT_SOURCEFILES += slip-bridge.c


PROJECT_SOURCEFILES += CoAPServer.c
PROJECT_SOURCEFILES += borderRouter.c

CFLAGS += -DUIP_FALLBACK_INTERFACE=rpl_interface

PREFIX = aaaa::1/64


APPS += er-coap
APPS += rest-engine
CONTIKI_TARGET_SOURCEFILES += sht25.c

REST_RESOURCES_DIR = ./resources

REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
PROJECTDIRS += $(REST_RESOURCES_DIR)
#PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)
PROJECT_SOURCEFILES += res-toggle.c

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include

$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 $(PREFIX)

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 $(PREFIX)



