CROSS_COMPILE := arm-linux-gnueabi-

CC      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)gcc

CFLAGS  := -Wall -std=gnu11 -g -D_REENTRANT


BLUEZ_BASE := $(shell ls ../ | grep -m 1 bluez)


BLUEZ_LIB_DIR := -L../$(BLUEZ_BASE)/lib/.libs/ -L../$(BLUEZ_BASE)/gdbus/.libs/ -L../$(BLUEZ_BASE)/src/.libs/
BLUEZ_LIBS := -lgdbus-internal -lshared-mainloop -lbluetooth-internal -lc
BLUEZ_INC := -I../$(BLUEZ_BASE)/


default : 
	$(CC) $(CFLAGS) transparent_service.c $(BLUEZ_LIBS) $(BLUEZ_LIB_DIR) $(BLUEZ_INC) -o transparent_service
	$(CC) $(CFLAGS) wifi_prov_service.c $(BLUEZ_LIBS) $(BLUEZ_LIB_DIR) $(BLUEZ_INC) -o wifi_prov_service

clean :
	$(RM) transparent_service wifi_prov_service
