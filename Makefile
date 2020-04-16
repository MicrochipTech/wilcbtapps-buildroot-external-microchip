BLUEZ_FOLDER := $(shell ls ../ | grep -m 1 bluez)

APP_FILES := transparent_service.c wifi_prov_service.c
DST_DIR := ../$(BLUEZ_FOLDER)/tools



default:
	for u in $(APP_FILES); do echo $$u; cp -f $$u $(DST_DIR); done
	python make_modify.py $(BLUEZ_FOLDER)
	cd ../$(BLUEZ_FOLDER) && $(MAKE)
	cp -rf $(DST_DIR)/transparent_service ../../target/usr/sbin/.
	cp -rf $(DST_DIR)/wifi_prov_service ../../target/usr/sbin/.
	cp -rf $(DST_DIR)/btgatt-server ../../target/usr/sbin/.



	
	
