#!/usr/local/bin/ python3
# -*- coding: utf-8 -*-

import requests, os, time
import xmltodict

MODEM_IP = "192.168.1.1"
MODEM_USER = "admin"
MODEM_PASS = "super0nline"

XML_APIS = [
  '/api/monitoring/traffic-statistics',
  '/api/dialup/connection',
  '/api/global/module-switch',
  '/api/net/current-plmn',
]

def get_xml_api(api):
	return requests.get("http://%s%s" % (MODEM_IP, api), auth=(MODEM_USER, MODEM_PASS)).text

def get_xml_apis():
	return [xmltodict.parse(get_xml_api(api)) for api in XML_APIS]

def get_xml_api_dict():
	return {api: xmltodict.parse(get_xml_api(api)) for api in XML_APIS}

# get table of values from xml api
def get_xml_api_table(api, table):
	return get_xml_api_dict()[api]['response'][table]


def printer():
	# '/api/monitoring/traffic-statistics': 
	# 'CurrentConnectTime': '16610', 'CurrentUpload': '187053961', 'CurrentDownload': '1535188838', 'CurrentDownloadRate': '17450', 'CurrentUploadRate': '7533', 'TotalUpload': '106977657518', 'TotalDownload': '2024904017917', 'TotalConnectTime': '49814755', 'showtraffic': '1'}}, 
	total_seconds = int(get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentConnectTime'))
	minutes, seconds = divmod(total_seconds, 60)
	hours, minutes = divmod(minutes, 60) # +5 minutes for timezone
	print("Current Connect Time: %d:%02d:%02d" % (hours, minutes+5, seconds))
	print("Current Connect Time Raw: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentConnectTime'))
	print("Current Upload: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentUpload'))
	print("Current Download: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentDownload'))
	print("Current Download Rate: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentDownloadRate'))
	print("Current Upload Rate: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentUploadRate'))
	print("Total Upload: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'TotalUpload'))
	print("Total Download: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'TotalDownload'))
	print("Total Connect Time: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'TotalConnectTime'))
	print("Show Traffic: ", get_xml_api_table('/api/monitoring/traffic-statistics', 'showtraffic'))

	# '/api/dialup/connection': 
	# 'RoamAutoConnectEnable': '0', 'MaxIdelTime': '0', 'ConnectMode': '0', 'MTU': '1500', 'auto_dial_switch': '1', 'pdp_always_on': '0'}}, 
	print("Roam Auto Connect Enable: ", get_xml_api_table('/api/dialup/connection', 'RoamAutoConnectEnable'))
	print("Max Idel Time: ", get_xml_api_table('/api/dialup/connection', 'MaxIdelTime'))
	print("Connect Mode: ", get_xml_api_table('/api/dialup/connection', 'ConnectMode'))
	print("MTU: ", get_xml_api_table('/api/dialup/connection', 'MTU'))
	print("Auto Dial Switch: ", get_xml_api_table('/api/dialup/connection', 'auto_dial_switch'))
	print("PDP Always On: ", get_xml_api_table('/api/dialup/connection', 'pdp_always_on'))

	# '/api/global/module-switch':
	# 'ussd_enabled': '0', 'ussdmenuswitch_enabled': '0', 'bbou_enabled': '1', 'sms_enabled': '1', 'sdcard_enabled': '0', 'wifi_enabled': '1', 'statistic_enabled': '1', 'help_enabled': '0', 'stk_enabled': '0', 'pb_enabled': '0', 'dlna_enabled': '1', 'wifioffload_enabled': '0', 'cradle_enabled': '1', 'multssid_enable': '1', 'ipv6_enabled': '1', 'monthly_volume_enabled': '1', 'powersave_enabled': '0', 'sntp_enabled': '0', 'timeserverdisplay_enabled': '1', 'sntpdisplay_enabled': '1', 'dstdisplay_enabled': '0', 'encrypt_enabled': '1', 'dataswitch_enabled': '1', 'apn_retry_enabled': '0', 'single_pdn_switch_enabled': '0', 'ddns_enabled': '1', 'sambashare_enabled': '1', 'poweroff_enabled': '0', 'bridge_enabled': '1', 'fw_macfilter_enabled': '1', 'zonetime_enabled': '0', 'diagnosis_enabled': '1', 'pcp_enabled': '0', 'localupdate_enabled': '1', 'cbs_enabled': '0', 'voip_enabled': '1', 'qrcode_enabled': '0', 'charger_enbaled': '0', 'vpn_enabled': '1', 'session_sms_enabled': '1', 'bestsignal_enable': '1', 'antenna_enabled': '1', 'static_route_enabled': '1', 'static_route6_enabled': '0', 'cwmp_enabled': '1', 'atp_enabled': '1', 'loginusername_enable': '1', 'autoreboot_enable': '1', 'ipmacdisplay_enable': '0', 'timemanualsetdisplay_enable': '0', 'volte_enabled': '0', 'get_pwd_enable': '1', 'hostinfo_enable': '1', 'chrlog_enable': '0', 'developermode_enable': '1', 'syslogexport_clear_update_enable': '0', 'gdpr_enabled': '0', 'gdpr_version': '1', 'hilink_enabled': '1', 'hilink_switch': '1', 'websocket_enabled': '1', 'hilink_online_update_enabled': '1', 'web_feature_enabled': '1', 'force_update_feature_enabled': '0', 'statistic_day_remind_enable': '0', 'safeinfo_enable': '0', 'mbbsec_enable': '1', 'ai_enable': '0', 'hostnamechange_enabled': '1', 'up_down_rate_enable': '1', 'automatically_update_enabled': '1', 'spreadname_enabled': '0', 'lan_ipmodeswitch_enabled': '0', 'china_region_enable': '0', 'aclEnabled': '1', 'staticdns_enabled': '0', 'nrProductEnable': '0', 'locationNominateEnable': '0', 'ledCtrlEnable': '0', 'oneKeyCheckEnable': '1', 'urlmenudisplay_enable': '0', 'roamdisplay_enable': '1', 'dialupapn_enable': '1'}}, 
	print("USSD Enabled: ", get_xml_api_table('/api/global/module-switch', 'ussd_enabled'))
	print("USSD Menu Switch Enabled: ", get_xml_api_table('/api/global/module-switch', 'ussdmenuswitch_enabled'))
	print("BBOU Enabled: ", get_xml_api_table('/api/global/module-switch', 'bbou_enabled'))
	print("SMS Enabled: ", get_xml_api_table('/api/global/module-switch', 'sms_enabled'))
	print("SD Card Enabled: ", get_xml_api_table('/api/global/module-switch', 'sdcard_enabled'))
	print("WiFi Enabled: ", get_xml_api_table('/api/global/module-switch', 'wifi_enabled'))
	print("Statistic Enabled: ", get_xml_api_table('/api/global/module-switch', 'statistic_enabled'))
	print("Help Enabled: ", get_xml_api_table('/api/global/module-switch', 'help_enabled'))
	print("STK Enabled: ", get_xml_api_table('/api/global/module-switch', 'stk_enabled'))
	print("PB Enabled: ", get_xml_api_table('/api/global/module-switch', 'pb_enabled'))
	print("DLNA Enabled: ", get_xml_api_table('/api/global/module-switch', 'dlna_enabled'))
	print("WiFi Offload Enabled: ", get_xml_api_table('/api/global/module-switch', 'wifioffload_enabled'))
	print("Cradle Enabled: ", get_xml_api_table('/api/global/module-switch', 'cradle_enabled'))
	print("MultSSID Enable: ", get_xml_api_table('/api/global/module-switch', 'multssid_enable'))
	print("IPv6 Enabled: ", get_xml_api_table('/api/global/module-switch', 'ipv6_enabled'))
	print("Monthly Volume Enabled: ", get_xml_api_table('/api/global/module-switch', 'monthly_volume_enabled'))
	print("Power Save Enabled: ", get_xml_api_table('/api/global/module-switch', 'powersave_enabled'))
	print("SNTP Enabled: ", get_xml_api_table('/api/global/module-switch', 'sntp_enabled'))
	print("Time Server Display Enabled: ", get_xml_api_table('/api/global/module-switch', 'timeserverdisplay_enabled'))
	print("SNTP Display Enabled: ", get_xml_api_table('/api/global/module-switch', 'sntpdisplay_enabled'))
	print("DST Display Enabled: ", get_xml_api_table('/api/global/module-switch', 'dstdisplay_enabled'))
	print("Encrypt Enabled: ", get_xml_api_table('/api/global/module-switch', 'encrypt_enabled'))
	print("Data Switch Enabled: ", get_xml_api_table('/api/global/module-switch', 'dataswitch_enabled'))
	
  # 'apn_retry_enabled': '0', 'single_pdn_switch_enabled': '0', 'ddns_enabled': '1', 'sambashare_enabled': '1', 'poweroff_enabled': '0', 'bridge_enabled': '1', 'fw_macfilter_enabled': '1', 'zonetime_enabled': '0', 'diagnosis_enabled': '1', 'pcp_enabled': '0', 'localupdate_enabled': '1', 'cbs_enabled': '0', 'voip_enabled': '1', 'qrcode_enabled': '0', 'charger_enbaled': '0', 'vpn_enabled': '1', 'session_sms_enabled': '1', 'bestsignal_enable': '1', 'antenna_enabled': '1', 'static_route_enabled': '1', 'static_route6_enabled': '0', 'cwmp_enabled': '1', 'atp_enabled': '1', 'loginusername_enable': '1', 'autoreboot_enable': '1', 'ipmacdisplay_enable': '0', 'timemanualsetdisplay_enable': '0', 'volte_enabled': '0', 'get_pwd_enable': '1', 'hostinfo_enable': '1', 'chrlog_enable': '0', 'developermode_enable': '1', 'syslogexport_clear_update_enable': '0', 'gdpr_enabled': '0', 'gdpr_version': '1', 'hilink_enabled': '1', 'hilink_switch': '1', 'websocket_enabled': '1', 'hilink_online_update_enabled': '1', 'web_feature_enabled': '1', 'force_update_feature_enabled': '0', 'statistic_day_remind_enable': '0', 'safeinfo_enable': '0', 'mbbsec_enable': '1', 'ai_enable': '0', 'hostnamechange_enabled': '1', 'up_down_rate_enable': '1', 'automatically_update_enabled': '1', 'spreadname_enabled': '0', 'lan_ipmodeswitch_enabled': '0', 'china_region_enable': '0', 'aclEnabled': '1', 'staticdns_enabled': '0', 'nrProductEnable': '0', 'locationNominateEnable': '0', 'ledCtrlEnable': '0', 'oneKeyCheckEnable': '1', 'urlmenudisplay_enable': '0', 'roamdisplay_enable': '1', 'dialupapn_enable': '1'}
	print("APN Retry Enabled: ", get_xml_api_table('/api/global/module-switch', 'apn_retry_enabled'))
	print("Single PDN Switch Enabled: ", get_xml_api_table('/api/global/module-switch', 'single_pdn_switch_enabled'))
	print("DDNS Enabled: ", get_xml_api_table('/api/global/module-switch', 'ddns_enabled'))
	print("Samba Share Enabled: ", get_xml_api_table('/api/global/module-switch', 'sambashare_enabled'))
	print("Power Off Enabled: ", get_xml_api_table('/api/global/module-switch', 'poweroff_enabled'))
	print("Bridge Enabled: ", get_xml_api_table('/api/global/module-switch', 'bridge_enabled'))
	print("Firewall MAC Filter Enabled: ", get_xml_api_table('/api/global/module-switch', 'fw_macfilter_enabled'))
	print("Zone Time Enabled: ", get_xml_api_table('/api/global/module-switch', 'zonetime_enabled'))
	print("Diagnosis Enabled: ", get_xml_api_table('/api/global/module-switch', 'diagnosis_enabled'))
	print("PCP Enabled: ", get_xml_api_table('/api/global/module-switch', 'pcp_enabled'))
	print("Local Update Enabled: ", get_xml_api_table('/api/global/module-switch', 'localupdate_enabled'))
	print("CBS Enabled: ", get_xml_api_table('/api/global/module-switch', 'cbs_enabled'))
	print("VoIP Enabled: ", get_xml_api_table('/api/global/module-switch', 'voip_enabled'))
	print("QR Code Enabled: ", get_xml_api_table('/api/global/module-switch', 'qrcode_enabled'))
	print("Charger Enabled: ", get_xml_api_table('/api/global/module-switch', 'charger_enbaled'))
	print("VPN Enabled: ", get_xml_api_table('/api/global/module-switch', 'vpn_enabled'))
	print("Session SMS Enabled: ", get_xml_api_table('/api/global/module-switch', 'session_sms_enabled'))
	print("Best Signal Enable: ", get_xml_api_table('/api/global/module-switch', 'bestsignal_enable'))
	print("Antenna Enabled: ", get_xml_api_table('/api/global/module-switch', 'antenna_enabled'))
	print("Static Route Enabled: ", get_xml_api_table('/api/global/module-switch', 'static_route_enabled'))
	print("Static Route6 Enabled: ", get_xml_api_table('/api/global/module-switch', 'static_route6_enabled'))
	print("CWMP Enabled: ", get_xml_api_table('/api/global/module-switch', 'cwmp_enabled'))
	print("ATP Enabled: ", get_xml_api_table('/api/global/module-switch', 'atp_enabled'))
	print("Login Username Enable: ", get_xml_api_table('/api/global/module-switch', 'loginusername_enable'))
	print("Auto Reboot Enable: ", get_xml_api_table('/api/global/module-switch', 'autoreboot_enable'))
	print("IP MAC Display Enable: ", get_xml_api_table('/api/global/module-switch', 'ipmacdisplay_enable'))
	print("Time Manual Set Display Enable: ", get_xml_api_table('/api/global/module-switch', 'timemanualsetdisplay_enable'))
	print("VoLTE Enabled: ", get_xml_api_table('/api/global/module-switch', 'volte_enabled'))

	# 'hostinfo_enable': '1', 'chrlog_enable': '0', 'developermode_enable': '1', 'syslogexport_clear_update_enable': '0', 'gdpr_enabled': '0', 'gdpr_version': '1', 'hilink_enabled': '1', 'hilink_switch': '1', 'websocket_enabled': '1', 'hilink_online_update_enabled': '1', 'web_feature_enabled': '1', 'force_update_feature_enabled': '0', 'statistic_day_remind_enable': '0', 'safeinfo_enable': '0', 'mbbsec_enable': '1', 'ai_enable': '0', 'hostnamechange_enabled': '1', 'up_down_rate_enable': '1', 'automatically_update_enabled': '1', 'spreadname_enabled': '0', 'lan_ipmodeswitch_enabled': '0', 'china_region_enable': '0', 'aclEnabled': '1', 'staticdns_enabled': '0', 'nrProductEnable': '0', 'locationNominateEnable': '0', 'ledCtrlEnable': '0', 'oneKeyCheckEnable': '1', 'urlmenudisplay_enable': '0', 'roamdisplay_enable': '1', 'dialupapn_enable': '1'}
	print("Host Info Enabled: ", get_xml_api_table('/api/global/module-switch', 'hostinfo_enable'))
	print("Chr Log Enabled: ", get_xml_api_table('/api/global/module-switch', 'chrlog_enable'))
	print("Developer Mode Enabled: ", get_xml_api_table('/api/global/module-switch', 'developermode_enable'))
	print("Syslog Export Clear Update Enabled: ", get_xml_api_table('/api/global/module-switch', 'syslogexport_clear_update_enable'))
	print("GDPR Enabled: ", get_xml_api_table('/api/global/module-switch', 'gdpr_enabled'))
	print("GDPR Version: ", get_xml_api_table('/api/global/module-switch', 'gdpr_version'))
	print("HiLink Enabled: ", get_xml_api_table('/api/global/module-switch', 'hilink_enabled'))
	print("HiLink Switch: ", get_xml_api_table('/api/global/module-switch', 'hilink_switch'))
	print("WebSocket Enabled: ", get_xml_api_table('/api/global/module-switch', 'websocket_enabled'))
	print("HiLink Online Update Enabled: ", get_xml_api_table('/api/global/module-switch', 'hilink_online_update_enabled'))
	print("Web Feature Enabled: ", get_xml_api_table('/api/global/module-switch', 'web_feature_enabled'))
	print("Force Update Feature Enabled: ", get_xml_api_table('/api/global/module-switch', 'force_update_feature_enabled'))
	print("Statistic Day Remind Enable: ", get_xml_api_table('/api/global/module-switch', 'statistic_day_remind_enable'))
	print("Safe Info Enable: ", get_xml_api_table('/api/global/module-switch', 'safeinfo_enable'))
	print("MBB Sec Enable: ", get_xml_api_table('/api/global/module-switch', 'mbbsec_enable'))
	print("AI Enable: ", get_xml_api_table('/api/global/module-switch', 'ai_enable'))
	print("Hostname Change Enabled: ", get_xml_api_table('/api/global/module-switch', 'hostnamechange_enabled'))
	print("Up Down Rate Enable: ", get_xml_api_table('/api/global/module-switch', 'up_down_rate_enable'))
	print("Automatically Update Enabled: ", get_xml_api_table('/api/global/module-switch', 'automatically_update_enabled'))
	print("Spread Name Enabled: ", get_xml_api_table('/api/global/module-switch', 'spreadname_enabled'))
	print("LAN IP Mode Switch Enabled: ", get_xml_api_table('/api/global/module-switch', 'lan_ipmodeswitch_enabled'))
	print("China Region Enable: ", get_xml_api_table('/api/global/module-switch', 'china_region_enable'))
	print("ACL Enabled: ", get_xml_api_table('/api/global/module-switch', 'aclEnabled'))
	print("Static DNS Enabled: ", get_xml_api_table('/api/global/module-switch', 'staticdns_enabled'))
	print("NR Product Enable: ", get_xml_api_table('/api/global/module-switch', 'nrProductEnable'))
	print("Location Nominate Enable: ", get_xml_api_table('/api/global/module-switch', 'locationNominateEnable'))
	print("LED Ctrl Enable: ", get_xml_api_table('/api/global/module-switch', 'ledCtrlEnable'))
	print("One Key Check Enable: ", get_xml_api_table('/api/global/module-switch', 'oneKeyCheckEnable'))
	print("URL Menu Display Enable: ", get_xml_api_table('/api/global/module-switch', 'urlmenudisplay_enable'))
	print("Roam Display Enable: ", get_xml_api_table('/api/global/module-switch', 'roamdisplay_enable'))
	print("Dial Up APN Enable: ", get_xml_api_table('/api/global/module-switch', 'dialupapn_enable'))
	
	# '/api/net/current-plmn': 
	#	'State': '0', 'FullName': 'TURKCELL', 'ShortName': 'TURKCELL', 'Numeric': '28601', 'Rat': '7', 'Spn': None}}, 
	print("Current PLMN Status: ", get_xml_api_table('/api/net/current-plmn', 'State'))
	print("Current PLMN FullName: ", get_xml_api_table('/api/net/current-plmn', 'FullName'))
	print("Current PLMN ShortName: ", get_xml_api_table('/api/net/current-plmn', 'ShortName'))
	print("Current PLMN Numeric: ", get_xml_api_table('/api/net/current-plmn', 'Numeric'))
	print("Current PLMN Rat: ", get_xml_api_table('/api/net/current-plmn', 'Rat'))
	print("Current PLMN Spn: ", get_xml_api_table('/api/net/current-plmn', 'Spn'))
	


def printer_short():
  
	print("\n Current PLMN FullName :", get_xml_api_table('/api/net/current-plmn', 'FullName'))
 
	current_upload = get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentUpload')
	c_upload = int(current_upload)
	c_upload = c_upload / 1024 / 1024 / 1024
	c_upload = round(c_upload, 4)
	print("\n Current Upload   :", c_upload, "GB")
 
	current_download = get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentDownload') 
	c_download = int(current_download)
	c_download = c_download / 1024 / 1024 / 1024
	c_download = round(c_download, 4)
	print(" Current Download :", c_download, "GB")
 

	current_upload_rate = get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentUploadRate')
	print("\n Curr Upload Rate   :", current_upload_rate, "Kbps")

	current_download_rate = get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentDownloadRate')
	print(" Curr Download Rate :", current_download_rate, "Kbps")
	
	total_upload = get_xml_api_table('/api/monitoring/traffic-statistics', 'TotalUpload')
	upload = int(total_upload) / 1024 / 1024 / 1024 / 1024
	upload = round(upload, 4)
	print("\n Total Upload   :", upload, "GB")

	
	total_download = get_xml_api_table('/api/monitoring/traffic-statistics', 'TotalDownload')
	download = int(total_download) / 1024 / 1024 / 1024
	download = round(download, 4)
	print(" Total Download :", download, "GB")
 
 
	total = upload + download
	print(" Total Traffic  :", total, "GB")
 
	total_seconds = int(get_xml_api_table('/api/monitoring/traffic-statistics', 'CurrentConnectTime'))
	minutes, seconds = divmod(total_seconds, 60)
	hours, minutes = divmod(minutes, 60) # +5 minutes for timezone
	print("\n Curr  Connect Time : %d:%02d:%02d" % (hours, minutes+5, seconds))
 
	total_time_seconds = int(get_xml_api_table('/api/monitoring/traffic-statistics', 'TotalConnectTime'))
	total_time_minutes, total_time_seconds = divmod(total_time_seconds, 60)
	total_time_hours, total_time_minutes = divmod(total_time_minutes, 60) # +5 minutes for timezone
	total_time_days, total_time_hours = divmod(total_time_hours, 24)
	print(" Total Connect Time : %ddays" % (total_time_days))

	print("\n")
if __name__ == '__main__':
  
  printer_short()
  
  '''
		while True:
			printer_short()
			time.sleep(10)
			os.system('clear')
	'''  
 # start date: 25/11/2022
 
