/*
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2014  Google Inc.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"

#include "src/shared/mainloop.h"
#include "src/shared/util.h"
#include "src/shared/att.h"
#include "src/shared/queue.h"
#include "src/shared/timeout.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"

#define UUID_GAP			0x1800
#define UUID_GATT			0x1801

//Service 1 Primary service Wifi connection service
const uint128_t prim_wifi_conn_uuid = {
	.data = { 0x77, 0x88, 0x00, 0x01, 0xd2, 0x29, 0x11, 0xe4,0x86, 0x89, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
        
const uint128_t wifi_conn_state_char_uuid = {
	.data = { 0x77, 0x88, 0x00, 0x02, 0xd2, 0x29, 0x11, 0xe4,0x86, 0x89, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
        
const uint128_t wifi_ap_parm_char_uuid = {
	.data = { 0x77, 0x88, 0x00, 0x03, 0xd2, 0x29, 0x11, 0xe4,0x86, 0x89, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};


//Service 2 primary service wifi scan connection service
const uint128_t prim_wifi_scan_uuid = {
	.data = { 0xfb, 0x8c, 0x00, 0x01, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_scanmode_char_uuid = {
         .data = { 0xfb, 0x8c, 0x00, 0x02, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
        
const uint128_t wifi_apscan_count_char_uuid = {
	.data = { 0xfb, 0x8c, 0x00, 0x03, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap1_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap2_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap3_char_uuid = {
        .data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
	
const uint128_t wifi_ap4_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap5_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap6_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap7_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
	
const uint128_t wifi_ap8_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};	

const uint128_t wifi_ap9_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
	
const uint128_t wifi_ap10_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
	
const uint128_t wifi_ap11_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap12_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap13_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap14_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

const uint128_t wifi_ap15_char_uuid = {
	.data = { 0xfb, 0x8c, 0x01, 0x00, 0xd2, 0x24, 0x11, 0xe4,0x85, 0xa1, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};
	
#define ATT_CID 4

#define PRLOG(...) \
	do { \
		printf(__VA_ARGS__); \
		print_prompt(); \
	} while (0)

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define COLOR_OFF	"\x1B[0m"
#define COLOR_RED	"\x1B[0;91m"
#define COLOR_GREEN	"\x1B[0;92m"
#define COLOR_YELLOW	"\x1B[0;93m"
#define COLOR_BLUE	"\x1B[0;94m"
#define COLOR_MAGENTA	"\x1B[0;95m"
#define COLOR_BOLDGRAY	"\x1B[1;30m"
#define COLOR_BOLDWHITE	"\x1B[1;37m"

static const char test_device_name[] = "Very Long Test Device Name For Testing "
				"ATT Protocol Operations On GATT Server";
static bool verbose = false;

struct server {
	int fd;
	struct bt_att *att;
	struct gatt_db *db;
	struct bt_gatt_server *gatt;

	uint8_t *device_name;
	size_t name_len;

	uint16_t gatt_svc_chngd_handle;
	bool svc_chngd_enabled;
	//service 1
	uint16_t wifi_conn_handle;
	uint16_t wifi_conn_state_handle;
	//service 2
	uint16_t wifi_scan_handle;	
	uint16_t wifi_scan_mode_c;

	bool service_visible;
};

static void print_prompt(void)
{
	printf(COLOR_BLUE "[GATT server]" COLOR_OFF "# ");
	fflush(stdout);
}

static void att_disconnect_cb(int err, void *user_data)
{
	printf("Device disconnected: %s\n", strerror(err));

	mainloop_quit();
}

static void att_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG(COLOR_BOLDGRAY "%s" COLOR_BOLDWHITE "%s\n" COLOR_OFF, prefix,
									str);
}

static void gatt_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG(COLOR_GREEN "%s%s\n" COLOR_OFF, prefix, str);
}

static void gap_device_name_read_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	struct server *server = user_data;
	uint8_t error = 0;
	size_t len = 0;
	const uint8_t *value = NULL;

	PRLOG("GAP Device Name Read called\n");

	len = server->name_len;

	if (offset > len) {
		error = BT_ATT_ERROR_INVALID_OFFSET;
		goto done;
	}

	len -= offset;
	value = len ? &server->device_name[offset] : NULL;

done:
	gatt_db_attribute_read_result(attrib, id, error, value, len);
}

static void gap_device_name_write_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					const uint8_t *value, size_t len,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	struct server *server = user_data;
	uint8_t error = 0;

	PRLOG("GAP Device Name Write called\n");

	/* If the value is being completely truncated, clean up and return */
	if (!(offset + len)) {
		free(server->device_name);
		server->device_name = NULL;
		server->name_len = 0;
		goto done;
	}

	/* Implement this as a variable length attribute value. */
	if (offset > server->name_len) {
		error = BT_ATT_ERROR_INVALID_OFFSET;
		goto done;
	}

	if (offset + len != server->name_len) {
		uint8_t *name;

		name = realloc(server->device_name, offset + len);
		if (!name) {
			error = BT_ATT_ERROR_INSUFFICIENT_RESOURCES;
			goto done;
		}

		server->device_name = name;
		server->name_len = offset + len;
	}

	if (value)
		memcpy(server->device_name + offset, value, len);

done:
	gatt_db_attribute_write_result(attrib, id, error);
}

static void gap_device_name_ext_prop_read_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	uint8_t value[2];

	PRLOG("Device Name Extended Properties Read called\n");

	value[0] = BT_GATT_CHRC_EXT_PROP_RELIABLE_WRITE;
	value[1] = 0;

	gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

static void gatt_service_changed_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	PRLOG("Service Changed Read called\n");

	gatt_db_attribute_read_result(attrib, id, 0, NULL, 0);
}

static void gatt_svc_chngd_ccc_read_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	struct server *server = user_data;
	uint8_t value[2];

	PRLOG("Service Changed CCC Read called\n");

	value[0] = server->svc_chngd_enabled ? 0x02 : 0x00;
	value[1] = 0x00;

	gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

static void gatt_svc_chngd_ccc_write_cb(struct gatt_db_attribute *attrib,
					unsigned int id, uint16_t offset,
					const uint8_t *value, size_t len,
					uint8_t opcode, struct bt_att *att,
					void *user_data)
{
	struct server *server = user_data;
	uint8_t ecode = 0;

	PRLOG("Service Changed CCC Write called\n");

	if (!value || len != 2) {
		ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
		goto done;
	}

	if (offset) {
		ecode = BT_ATT_ERROR_INVALID_OFFSET;
		goto done;
	}

	if (value[0] == 0x00)
		server->svc_chngd_enabled = false;
	else if (value[0] == 0x02)
		server->svc_chngd_enabled = true;
	else
		ecode = 0x80;

	PRLOG("Service Changed Enabled: %s\n",
				server->svc_chngd_enabled ? "true" : "false");

done:
	gatt_db_attribute_write_result(attrib, id, ecode);
}

void update_wpa_supp(char ssid[],char psk[])
{
	int fd=0,sz=0;
	
	fd = open("/etc/wpa_supplicant.conf", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd < 0)
	{
		perror("r1");
		exit(1);
  	}

	printf( "ssid=%s ssid_len=%d\n", ssid,strlen(ssid) );
	sz = write(fd, "ctrl_interface=/var/run/wpa_supplicant\n", strlen("ctrl_interface=/var/run/wpa_supplicant\n"));
	sz = write(fd, "ap_scan=1\n", strlen("ap_scan=1\n"));
	sz = write(fd, "network={\n", strlen("network={\n"));
	sz = write(fd, "ssid=", strlen("ssid="));
	sz = write(fd, ssid, strlen(ssid));
	sz = write(fd, "\n", strlen("\n"));
	printf( "psk=%s psklen=%d\n", psk,strlen(psk) );
  
	if(strlen(psk) > 3)
	{
		sz = write(fd, "psk=", strlen("psk="));
		sz = write(fd, psk, strlen(psk));
		sz = write(fd, "\n", strlen("\n"));
	}else
	{
		sz = write(fd, "key_mgmt=NONE\n", strlen("key_mgmt=NONE\n"));
	}

	sz = write(fd, "}\n", strlen("}\n"));
	close(fd);
}

#define START_OF_HEADING 0x01
#define START_OF_TEXT 0x02

static void process_wifi_prov_data(const uint8_t *value,size_t len)
{
	size_t ssid_len=0,psk_len=0,idx1=0,val_idx=0;
	char ssid[32],psk[64];
	char a='"';

	
	if((value[val_idx] == START_OF_HEADING)|| (value[val_idx] == START_OF_TEXT)) //value[0]
	{
		val_idx ++;

		memset(ssid,0,32);
		memset(psk,0,64);

		ssid_len = value[val_idx++];     //value[1]
		ssid[0] = a;			//storing special character " for wpa_supplicant formating
		for(idx1=1;idx1 <= ssid_len ; ) {
			ssid[idx1++]=value[val_idx++];
		}
		ssid[idx1++] = a;	//storing special character " for wpa_supplicant formating
		ssid[idx1] ='\0';
		while((value[val_idx++] == 0x00) && (val_idx < len)); // remove all the null
		
		if(val_idx < len)
		{
			psk_len = value[val_idx-1];
			psk[0] = a;		//storing special character " for wpa_supplicant formating
			for(idx1=1;idx1 <= psk_len ;) {
				psk[idx1++]=value[val_idx++];
			}
			psk[idx1++] = a;	//storing special character " for wpa_supplicant formating
			psk[idx1] ='\0';
		}
				
	}
	
	update_wpa_supp(ssid,psk);
	
	system("wpa_cli -i wlan0 reconfigure"); //update received configuration of wpa_supplicant
}
static void read_cb(struct gatt_db_attribute *attrib, unsigned int id,
                        uint16_t offset, uint8_t opcode, struct bt_att *att,
                        void *user_data)
{
	printf("read_cb received \n");
}
static void write_cb(struct gatt_db_attribute *attrib, unsigned int id,
                        uint16_t offset, const uint8_t *value, size_t len,
                        uint8_t opcode, struct bt_att *att, void *user_data)
{
	process_wifi_prov_data(value,len);
	
}

static void populate_gap_service(struct server *server)
{
	bt_uuid_t uuid;
	struct gatt_db_attribute *service, *tmp;
	uint16_t appearance;

	/* Add the GAP service */
	bt_uuid16_create(&uuid, UUID_GAP);
	service = gatt_db_add_service(server->db, &uuid, true, 6);

	/*
	 * Device Name characteristic. Make the value dynamically read and
	 * written via callbacks.
	 */
	bt_uuid16_create(&uuid, GATT_CHARAC_DEVICE_NAME);
	gatt_db_service_add_characteristic(service, &uuid,
					BT_ATT_PERM_READ,
					BT_GATT_CHRC_PROP_READ,
					gap_device_name_read_cb,
					NULL,
					server);
	bt_uuid16_create(&uuid, GATT_CHARAC_EXT_PROPER_UUID);
	gatt_db_service_add_descriptor(service, &uuid, BT_ATT_PERM_READ,
					gap_device_name_ext_prop_read_cb,
					NULL, server);
	/*
	 * Appearance characteristic. Reads and writes should obtain the value
	 * from the database.
	 */
	bt_uuid16_create(&uuid, GATT_CHARAC_APPEARANCE);
	tmp = gatt_db_service_add_characteristic(service, &uuid,
							BT_ATT_PERM_READ,
							BT_GATT_CHRC_PROP_READ,
							NULL, NULL, server);
	gatt_db_service_set_active(service, true);
}

static void populate_gatt_service(struct server *server)
{
	bt_uuid_t uuid;
	struct gatt_db_attribute *service, *svc_chngd;

	/* Add the GATT service */
	bt_uuid16_create(&uuid, UUID_GATT);
	service = gatt_db_add_service(server->db, &uuid, true, 4);

	bt_uuid16_create(&uuid, GATT_CHARAC_SERVICE_CHANGED);
	svc_chngd = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_READ,
			BT_GATT_CHRC_PROP_READ,
			gatt_service_changed_cb,
			NULL, server);
	server->gatt_svc_chngd_handle = gatt_db_attribute_get_handle(svc_chngd);
	gatt_db_service_set_active(service, true);
}

static void populate_wifi_conn_service(struct server *server)
{
	bt_uuid_t uuid;
        uint128_t prim_wifi_conn_uuid_h, wifi_conn_state_char_uuid_h, wifi_ap_parm_char_uuid_h;
	struct gatt_db_attribute *service, *wifi_conn_state, *wifi_ap_param;
	
	btoh128(&prim_wifi_conn_uuid, &prim_wifi_conn_uuid_h);
        bt_uuid128_create(&uuid, prim_wifi_conn_uuid_h);
	service = gatt_db_add_service(server->db, &uuid, true, 8);
	server->wifi_conn_handle = gatt_db_attribute_get_handle(service);

        btoh128(&wifi_conn_state_char_uuid, &wifi_conn_state_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_conn_state_char_uuid_h);
	wifi_conn_state = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_NONE,
						BT_GATT_CHRC_PROP_NOTIFY | BT_GATT_CHRC_PROP_INDICATE |
						BT_GATT_CHRC_PROP_READ   | BT_GATT_CHRC_PROP_WRITE,
						NULL, NULL, NULL);
	server->wifi_conn_state_handle = gatt_db_attribute_get_handle(wifi_conn_state);

	bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
        gatt_db_service_add_descriptor(service, &uuid,
                                        BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                                        read_cb,
                                        NULL, server);
	
        btoh128(&wifi_ap_parm_char_uuid, &wifi_ap_parm_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap_parm_char_uuid_h);
	wifi_ap_param = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_WRITE | BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_WRITE | BT_GATT_CHRC_PROP_READ,
						read_cb, write_cb, server);
	if (server->service_visible)
	gatt_db_service_set_active(service, true);
}

static void populate_wifi_scan_service(struct server *server)
{
	bt_uuid_t uuid;
	uint128_t prim_wifi_scan_uuid_h,wifi_scanmode_char_uuid_h,wifi_apscan_count_char_uuid_h,wifi_ap1_char_uuid_h, wifi_ap2_char_uuid_h;
	uint128_t wifi_ap3_char_uuid_h,wifi_ap4_char_uuid_h,wifi_ap5_char_uuid_h,wifi_ap6_char_uuid_h,wifi_ap7_char_uuid_h;
	uint128_t wifi_ap8_char_uuid_h,wifi_ap9_char_uuid_h,wifi_ap10_char_uuid_h,wifi_ap11_char_uuid_h,wifi_ap12_char_uuid_h;
	uint128_t wifi_ap13_char_uuid_h,wifi_ap14_char_uuid_h,wifi_ap15_char_uuid_h;
	struct gatt_db_attribute *service, *wifi_scan_mode_c, *wifi_ap_scan_count_c,*wifi_ap_c;
        
	btoh128(&prim_wifi_scan_uuid, &prim_wifi_scan_uuid_h);
        bt_uuid128_create(&uuid, prim_wifi_scan_uuid_h);
	service = gatt_db_add_service(server->db, &uuid, true, 64);
	server->wifi_scan_handle = gatt_db_attribute_get_handle(service);

        btoh128(&wifi_scanmode_char_uuid, &wifi_scanmode_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_scanmode_char_uuid_h);
	wifi_scan_mode_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_NONE,
						BT_GATT_CHRC_PROP_NOTIFY | BT_GATT_CHRC_PROP_INDICATE |
						BT_GATT_CHRC_PROP_READ   | BT_GATT_CHRC_PROP_WRITE,
						NULL, NULL, NULL);
	server->wifi_scan_mode_c = gatt_db_attribute_get_handle(wifi_scan_mode_c);
	
	bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
        gatt_db_service_add_descriptor(service, &uuid,
                                        BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                                        read_cb,
                                        NULL, server);
        
	btoh128(&wifi_apscan_count_char_uuid, &wifi_apscan_count_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_apscan_count_char_uuid_h);
	wifi_ap_scan_count_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap1_char_uuid, &wifi_ap1_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap1_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);

	btoh128(&wifi_ap2_char_uuid, &wifi_ap2_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap2_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);

	btoh128(&wifi_ap3_char_uuid, &wifi_ap3_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap3_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap4_char_uuid, &wifi_ap4_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap4_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);

	btoh128(&wifi_ap5_char_uuid, &wifi_ap5_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap5_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);

	btoh128(&wifi_ap6_char_uuid, &wifi_ap6_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap6_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);

	btoh128(&wifi_ap7_char_uuid, &wifi_ap7_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap7_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap8_char_uuid, &wifi_ap8_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap8_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap9_char_uuid, &wifi_ap9_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap9_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap10_char_uuid, &wifi_ap10_char_uuid_h);
	bt_uuid128_create(&uuid, wifi_ap10_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap11_char_uuid, &wifi_ap11_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap11_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap12_char_uuid, &wifi_ap12_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap12_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap13_char_uuid, &wifi_ap13_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap13_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap14_char_uuid, &wifi_ap14_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap14_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	
	btoh128(&wifi_ap15_char_uuid, &wifi_ap15_char_uuid_h);
        bt_uuid128_create(&uuid, wifi_ap15_char_uuid_h);
	wifi_ap_c = gatt_db_service_add_characteristic(service, &uuid,
						BT_ATT_PERM_READ,
						BT_GATT_CHRC_PROP_READ,
						read_cb, NULL, server);
	if (server->service_visible)
	gatt_db_service_set_active(service, true);
}

static void populate_db(struct server *server)
{
	populate_gap_service(server);
	populate_gatt_service(server);
	populate_wifi_conn_service(server);
	populate_wifi_scan_service(server);
}

static struct server *server_create(int fd, uint16_t mtu,bool service_visible)
{
	struct server *server;
	size_t name_len = strlen(test_device_name);

	server = new0(struct server, 1);
	if (!server) {
		fprintf(stderr, "Failed to allocate memory for server\n");
		return NULL;
	}

	server->att = bt_att_new(fd, false);
	if (!server->att) {
		fprintf(stderr, "Failed to initialze ATT transport layer\n");
		goto fail;
	}

	if (!bt_att_set_close_on_unref(server->att, true)) {
		fprintf(stderr, "Failed to set up ATT transport layer\n");
		goto fail;
	}

	if (!bt_att_register_disconnect(server->att, att_disconnect_cb, NULL,
									NULL)) {
		fprintf(stderr, "Failed to set ATT disconnect handler\n");
		goto fail;
	}

	server->name_len = name_len + 1;
	server->device_name = malloc(name_len + 1);
	if (!server->device_name) {
		fprintf(stderr, "Failed to allocate memory for device name\n");
		goto fail;
	}

	memcpy(server->device_name, test_device_name, name_len);
	server->device_name[name_len] = '\0';

	server->fd = fd;
	server->db = gatt_db_new();
	if (!server->db) {
		fprintf(stderr, "Failed to create GATT database\n");
		goto fail;
	}

	server->gatt = bt_gatt_server_new(server->db, server->att, mtu, 0);
	if (!server->gatt) {
		fprintf(stderr, "Failed to create GATT server\n");
		goto fail;
	}

	server->service_visible = service_visible;
	if (verbose) {
		bt_att_set_debug(server->att, att_debug_cb, "att: ", NULL);
		bt_gatt_server_set_debug(server->gatt, gatt_debug_cb,
							"server: ", NULL);
	}
	/* Random seed for generating fake Heart Rate measurements */
	srand(time(NULL));

	/* bt_gatt_server already holds a reference */
	populate_db(server);

	return server;

fail:
	gatt_db_unref(server->db);
	free(server->device_name);
	bt_att_unref(server->att);
	free(server);

	return NULL;
}

static void server_destroy(struct server *server)
{
	bt_gatt_server_unref(server->gatt);
	gatt_db_unref(server->db);
}

static void usage(void)
{
	printf("btgatt-server\n");
	printf("Usage:\n\tbtgatt-server [options]\n");

	printf("Options:\n"
		"\t-i, --index <id>\t\tSpecify adapter index, e.g. hci0\n"
		"\t-m, --mtu <mtu>\t\t\tThe ATT MTU to use\n"
		"\t-s, --security-level <sec>\tSet security level (low|"
								"medium|high)\n"
		"\t-t, --type [random|public] \t The source address type\n"
		"\t-v, --verbose\t\t\tEnable extra logging\n"
		"\t-r, --Wifi Provision service\t\tEnable WiFi Provision service\n"
		"\t-h, --help\t\t\tDisplay help\n");
}

static struct option main_options[] = {
	{ "index",		1, 0, 'i' },
	{ "mtu",		1, 0, 'm' },
	{ "security-level",	1, 0, 's' },
	{ "type",		1, 0, 't' },
	{ "verbose",		0, 0, 'v' },
	{ "wifi-prov",		0, 0, 'r' },
	{ "help",		0, 0, 'h' },
	{ }
};

static int l2cap_le_att_listen_and_accept(bdaddr_t *src, int sec,
							uint8_t src_type)
{
	int sk, nsk;
	struct sockaddr_l2 srcaddr, addr;
	socklen_t optlen;
	struct bt_security btsec;
	char ba[18];

	sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (sk < 0) {
		perror("Failed to create L2CAP socket");
		return -1;
	}

	/* Set up source address */
	memset(&srcaddr, 0, sizeof(srcaddr));
	srcaddr.l2_family = AF_BLUETOOTH;
	srcaddr.l2_cid = htobs(ATT_CID);
	srcaddr.l2_bdaddr_type = src_type;
	bacpy(&srcaddr.l2_bdaddr, src);

	if (bind(sk, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
		perror("Failed to bind L2CAP socket");
		goto fail;
	}

	/* Set the security level */
	memset(&btsec, 0, sizeof(btsec));
	btsec.level = sec;
	if (setsockopt(sk, SOL_BLUETOOTH, BT_SECURITY, &btsec,
							sizeof(btsec)) != 0) {
		fprintf(stderr, "Failed to set L2CAP security level\n");
		goto fail;
	}

	if (listen(sk, 10) < 0) {
		perror("Listening on socket failed");
		goto fail;
	}

	printf("Started listening on ATT channel. Waiting for connections\n");

	memset(&addr, 0, sizeof(addr));
	optlen = sizeof(addr);
	nsk = accept(sk, (struct sockaddr *) &addr, &optlen);
	if (nsk < 0) {
		perror("Accept failed");
		goto fail;
	}

	ba2str(&addr.l2_bdaddr, ba);
	printf("Connect from %s\n", ba);
	close(sk);

	return nsk;

fail:
	close(sk);
	return -1;
}

static void notify_usage(void)
{
	printf("Usage: notify [options] <value_handle> <value>\n"
					"Options:\n"
					"\t -i, --indicate\tSend indication\n"
					"e.g.:\n"
					"\tnotify 0x0001 00 01 00\n");
}

static struct option notify_options[] = {
	{ "indicate",	0, 0, 'i' },
	{ }
};

static bool parse_args(char *str, int expected_argc,  char **argv, int *argc)
{
	char **ap;

	for (ap = argv; (*ap = strsep(&str, " \t")) != NULL;) {
		if (**ap == '\0')
			continue;

		(*argc)++;
		ap++;

		if (*argc > expected_argc)
			return false;
	}

	return true;
}

static void conf_cb(void *user_data)
{
	PRLOG("Received confirmation\n");
}

static void cmd_notify(struct server *server, char *cmd_str)
{
	int opt, i;
	char *argvbuf[516];
	char **argv = argvbuf;
	int argc = 1;
	uint16_t handle;
	char *endptr = NULL;
	int length;
	uint8_t *value = NULL;
	bool indicate = false;

	if (!parse_args(cmd_str, 514, argv + 1, &argc)) {
		printf("Too many arguments\n");
		notify_usage();
		return;
	}

	optind = 0;
	argv[0] = "notify";
	while ((opt = getopt_long(argc, argv, "+i", notify_options,
								NULL)) != -1) {
		switch (opt) {
		case 'i':
			indicate = true;
			break;
		default:
			notify_usage();
			return;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1) {
		notify_usage();
		return;
	}

	handle = strtol(argv[0], &endptr, 16);
	if (!endptr || *endptr != '\0' || !handle) {
		printf("Invalid handle: %s\n", argv[0]);
		return;
	}

	length = argc - 1;

	if (length > 0) {
		if (length > UINT16_MAX) {
			printf("Value too long\n");
			return;
		}

		value = malloc(length);
		if (!value) {
			printf("Failed to construct value\n");
			return;
		}

		for (i = 1; i < argc; i++) {
			if (strlen(argv[i]) != 2) {
				printf("Invalid value byte: %s\n",
								argv[i]);
				goto done;
			}

			value[i-1] = strtol(argv[i], &endptr, 16);
			if (endptr == argv[i] || *endptr != '\0'
							|| errno == ERANGE) {
				printf("Invalid value byte: %s\n",
								argv[i]);
				goto done;
			}
		}
	}

	printf("send notification and indication update \n");
	if (indicate) {
		if (!bt_gatt_server_send_indication(server->gatt, handle,
							value, length,
							conf_cb, NULL, NULL))
			printf("Failed to initiate indication\n");
	} else if (!bt_gatt_server_send_notification(server->gatt, handle,
								value, length))
		printf("Failed to initiate notification\n");

done:
	free(value);
}

static void print_uuid(const bt_uuid_t *uuid)
{
	char uuid_str[MAX_LEN_UUID_STR];
	bt_uuid_t uuid128;

	bt_uuid_to_uuid128(uuid, &uuid128);
	bt_uuid_to_string(&uuid128, uuid_str, sizeof(uuid_str));

	printf("%s\n", uuid_str);
}

static void print_incl(struct gatt_db_attribute *attr, void *user_data)
{
	struct server *server = user_data;
	uint16_t handle, start, end;
	struct gatt_db_attribute *service;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_incl_data(attr, &handle, &start, &end))
		return;

	service = gatt_db_get_attribute(server->db, start);
	if (!service)
		return;

	gatt_db_attribute_get_service_uuid(service, &uuid);

	printf("\t  " COLOR_GREEN "include" COLOR_OFF " - handle: "
					"0x%04x, - start: 0x%04x, end: 0x%04x,"
					"uuid: ", handle, start, end);
	print_uuid(&uuid);
}

static void print_desc(struct gatt_db_attribute *attr, void *user_data)
{
	printf("\t\t  " COLOR_MAGENTA "descr" COLOR_OFF
					" - handle: 0x%04x, uuid: ",
					gatt_db_attribute_get_handle(attr));
	print_uuid(gatt_db_attribute_get_type(attr));
}

static void print_chrc(struct gatt_db_attribute *attr, void *user_data)
{
	uint16_t handle, value_handle;
	uint8_t properties;
	uint16_t ext_prop;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_char_data(attr, &handle,
								&value_handle,
								&properties,
								&ext_prop,
								&uuid))
		return;

	printf("\t  " COLOR_YELLOW "charac" COLOR_OFF
				" - start: 0x%04x, value: 0x%04x, "
				"props: 0x%02x, ext_prop: 0x%04x, uuid: ",
				handle, value_handle, properties, ext_prop);
	print_uuid(&uuid);

	gatt_db_service_foreach_desc(attr, print_desc, NULL);
}

static void print_service(struct gatt_db_attribute *attr, void *user_data)
{
	struct server *server = user_data;
	uint16_t start, end;
	bool primary;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_service_data(attr, &start, &end, &primary,
									&uuid))
		return;

	printf(COLOR_RED "service" COLOR_OFF " - start: 0x%04x, "
				"end: 0x%04x, type: %s, uuid: ",
				start, end, primary ? "primary" : "secondary");
	print_uuid(&uuid);

	gatt_db_service_foreach_incl(attr, print_incl, server);
	gatt_db_service_foreach_char(attr, print_chrc, NULL);

	printf("\n");
}

static void cmd_services(struct server *server, char *cmd_str)
{
	gatt_db_foreach_service(server->db, NULL, print_service, server);
}

static bool convert_sign_key(char *optarg, uint8_t key[16])
{
	int i;

	if (strlen(optarg) != 32) {
		printf("sign-key length is invalid\n");
		return false;
	}

	for (i = 0; i < 16; i++) {
		if (sscanf(optarg + (i * 2), "%2hhx", &key[i]) != 1)
			return false;
	}

	return true;
}

static void set_sign_key_usage(void)
{
	printf("Usage: set-sign-key [options]\nOptions:\n"
		"\t -c, --sign-key <remote csrk>\tRemote CSRK\n"
		"e.g.:\n"
		"\tset-sign-key -c D8515948451FEA320DC05A2E88308188\n");
}

static bool remote_counter(uint32_t *sign_cnt, void *user_data)
{
	static uint32_t cnt = 0;

	if (*sign_cnt < cnt)
		return false;

	cnt = *sign_cnt;

	return true;
}

static void cmd_set_sign_key(struct server *server, char *cmd_str)
{
	char *argv[3];
	int argc = 0;
	uint8_t key[16];

	memset(key, 0, 16);

	if (!parse_args(cmd_str, 2, argv, &argc)) {
		set_sign_key_usage();
		return;
	}

	if (argc != 2) {
		set_sign_key_usage();
		return;
	}

	if (!strcmp(argv[0], "-c") || !strcmp(argv[0], "--sign-key")) {
		if (convert_sign_key(argv[1], key))
			bt_att_set_remote_key(server->att, key, remote_counter,
									server);
	} else
		set_sign_key_usage();
}

static void cmd_help(struct server *server, char *cmd_str);

typedef void (*command_func_t)(struct server *server, char *cmd_str);

static struct {
	char *cmd;
	command_func_t func;
	char *doc;
} command[] = {
	{ "help", cmd_help, "\tDisplay help message" },
	{ "notify", cmd_notify, "\tSend handle-value notification" },
	{ "services", cmd_services, "\tEnumerate all services" },
	{ "set-sign-key", cmd_set_sign_key,
			"\tSet remote signing key for signed write command"},
	{ }
};

static void cmd_help(struct server *server, char *cmd_str)
{
	int i;

	printf("Commands:\n");
	for (i = 0; command[i].cmd; i++)
		printf("\t%-15s\t%s\n", command[i].cmd, command[i].doc);
}

static void prompt_read_cb(int fd, uint32_t events, void *user_data)
{
	ssize_t read;
	size_t len = 0;
	char *line = NULL;
	char *cmd = NULL, *args;
	struct server *server = user_data;
	int i;

	if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
		mainloop_quit();
		return;
	}

	read = getline(&line, &len, stdin);
	if (read < 0)
		return;

	if (read <= 1) {
		cmd_help(server, NULL);
		print_prompt();
		return;
	}

	line[read-1] = '\0';
	args = line;

	while ((cmd = strsep(&args, " \t")))
		if (*cmd != '\0')
			break;

	if (!cmd)
		goto failed;

	for (i = 0; command[i].cmd; i++) {
		if (strcmp(command[i].cmd, cmd) == 0)
			break;
	}

	if (command[i].cmd)
		command[i].func(server, args);
	else
		fprintf(stderr, "Unknown command: %s\n", line);

failed:
	print_prompt();

	free(line);
}

static void signal_cb(int signum, void *user_data)
{
	switch (signum) {
	case SIGINT:
	case SIGTERM:
		mainloop_quit();
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	int opt;
	bdaddr_t src_addr;
	int dev_id = -1;
	int fd;
	int sec = BT_SECURITY_LOW;
	uint8_t src_type = BDADDR_LE_PUBLIC;
	uint16_t mtu = 0;
	sigset_t mask;
	bool service_visible = false;
	struct server *server;

	printf("start of the main\n");
	while ((opt = getopt_long(argc, argv, "+hvrs:t:m:i:",
						main_options, NULL)) != -1) {
		switch (opt) {
		case 'h':
			usage();
			return EXIT_SUCCESS;
		case 'v':
			verbose = true;
			break;
		case 'r':
			service_visible = true;
			break;
		case 's':
			if (strcmp(optarg, "low") == 0)
				sec = BT_SECURITY_LOW;
			else if (strcmp(optarg, "medium") == 0)
				sec = BT_SECURITY_MEDIUM;
			else if (strcmp(optarg, "high") == 0)
				sec = BT_SECURITY_HIGH;
			else {
				fprintf(stderr, "Invalid security level\n");
				return EXIT_FAILURE;
			}
			break;
		case 't':
			if (strcmp(optarg, "random") == 0)
				src_type = BDADDR_LE_RANDOM;
			else if (strcmp(optarg, "public") == 0)
				src_type = BDADDR_LE_PUBLIC;
			else {
				fprintf(stderr,
					"Allowed types: random, public\n");
				return EXIT_FAILURE;
			}
			break;
		case 'm': {
			int arg;

			arg = atoi(optarg);
			if (arg <= 0) {
				fprintf(stderr, "Invalid MTU: %d\n", arg);
				return EXIT_FAILURE;
			}

			if (arg > UINT16_MAX) {
				fprintf(stderr, "MTU too large: %d\n", arg);
				return EXIT_FAILURE;
			}

			mtu = (uint16_t) arg;
			break;
		}
		case 'i':
			dev_id = hci_devid(optarg);
			if (dev_id < 0) {
				perror("Invalid adapter");
				return EXIT_FAILURE;
			}

			break;
		default:
			fprintf(stderr, "Invalid option: %c\n", opt);
			return EXIT_FAILURE;
		}
	}

	argc -= optind;
	argv -= optind;
	optind = 0;

	if (argc) {
		usage();
		return EXIT_SUCCESS;
	}

	if (dev_id == -1)
		bacpy(&src_addr, BDADDR_ANY);
	else if (hci_devba(dev_id, &src_addr) < 0) {
		perror("Adapter not available");
		return EXIT_FAILURE;
	}

	fd = l2cap_le_att_listen_and_accept(&src_addr, sec, src_type);
	if (fd < 0) {
		fprintf(stderr, "Failed to accept L2CAP ATT connection\n");
		return EXIT_FAILURE;
	}

	mainloop_init();

	server = server_create(fd, mtu, service_visible);
	if (!server) {
		close(fd);
		return EXIT_FAILURE;
	}

	if (mainloop_add_fd(fileno(stdin),
				EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR,
				prompt_read_cb, server, NULL) < 0) {
		fprintf(stderr, "Failed to initialize console\n");
		server_destroy(server);

		return EXIT_FAILURE;
	}

	printf("Running GATT server\n");

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);

	mainloop_run_with_signal(signal_cb, NULL);

	print_prompt();

	mainloop_run();

	printf("\n\nShutting down...\n");

	server_destroy(server);

	return EXIT_SUCCESS;
}
