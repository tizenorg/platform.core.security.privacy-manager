/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <PrivacyIdInfo.h>
#include <privacy_manager_client_types.h>
#include <PrivacyManagerTypes.h>
#include <dlog.h>
#include <set>
#include <Utils.h>


std::map <std::string, std::string> PrivacyIdInfo::m_privilegeToPrivacyMap;
std::map <std::string, std::string> PrivacyIdInfo::m_deviceCapToPrivacyMap;
bool PrivacyIdInfo:: m_isInitialized;


struct PrivacyPrivilegePair
{
	char privilege[128];
	char privacy[128];
};

struct PrivacyDeviceCapPair
{
	char DeviceCap[128];
	char privacy[128];
};
/*
static struct PrivacyPrivilegePair DeviceCapToPrivacyTable[] =
{
	{"bluetooth.admin", 			"http://tizen.org/privacy/bluetooth"},
	{"bluetooth.gap", 				"http://tizen.org/privacy/bluetooth"},
	{"bluetooth.spp", 				"http://tizen.org/privacy/bluetooth"},
	{"bluetoothmanager", 			"http://tizen.org/privacy/bluetooth"},

	{"calendar.read", 				"http://tizen.org/privacy/calender"},
	{"calendar.write", 				"http://tizen.org/privacy/calender"},

	{"contact.read", 				"http://tizen.org/privacy/contact"},
	{"contact.write", 				"http://tizen.org/privacy/contact"},

	{"messaging.read", 				"http://tizen.org/privacy/messaging"},
	{"messaging.write", 			"http://tizen.org/privacy/messaging"},
	{"messaging.send", 				"http://tizen.org/privacy/messaging"},

	{"nfc.admin", 					"http://tizen.org/privacy/nfc"},
	{"nfcmanager.cardemulation", 	"http://tizen.org/privacy/nfc"},
	{"nfc.common", 					"http://tizen.org/privacy/nfc"},
	{"nfc.p2p", 					"http://tizen.org/privacy/nfc"},
	{"nfc.tag", 					"http://tizen.org/privacy/nfc"},

	{"XMLHttpRequest", 				"http://tizen.org/privacy/internet"},
	{"externalNetworkAccess", 		"http://tizen.org/privacy/internet"},

	{"\0", "\0"}
};

static struct PrivacyPrivilegePair PrivilegeToPrivacyTable[] =
{
	{"http://tizen.org/privilege/bluetooth.admin", 			"http://tizen.org/privacy/bluetooth"},
	{"http://tizen.org/privilege/bluetooth.gap", 			"http://tizen.org/privacy/bluetooth"},
	{"http://tizen.org/privilege/bluetooth.health",			"http://tizen.org/privacy/bluetooth"},
	{"http://tizen.org/privilege/bluetooth.opp", 			"http://tizen.org/privacy/bluetooth"},
	{"http://tizen.org/privilege/bluetooth.spp", 			"http://tizen.org/privacy/bluetooth"},
	{"http://tizen.org/privilege/bluetoothmanager",			"http://tizen.org/privacy/bluetooth"},

	{"http://tizen.org/privilege/calendar.read", 			"http://tizen.org/privacy/calender"},
	{"http://tizen.org/privilege/calendar.write", 			"http://tizen.org/privacy/calender"},

	{"http://tizen.org/privilege/contact.read", 			"http://tizen.org/privacy/contact"},
	{"http://tizen.org/privilege/contact.write", 			"http://tizen.org/privacy/contact"},

	{"http://tizen.org/privilege/contextmanager.privacy", 	"http://tizen.org/privacy/context"},
	{"http://tizen.org/privilege/contextmanager.upload", 	"http://tizen.org/privacy/context"},

	{"http://tizen.org/privilege/location", 				"http://tizen.org/privacy/location"},

	{"http://tizen.org/privilege/messaging.read", 			"http://tizen.org/privacy/messaging"},
	{"http://tizen.org/privilege/messaging.write", 			"http://tizen.org/privacy/messaging"},

	{"http://tizen.org/privilege/nfc.admin", 				"http://tizen.org/privacy/nfc"},
	{"http://tizen.org/privilege/nfcmanager.cardemulation", "http://tizen.org/privacy/nfc"},
	{"http://tizen.org/privilege/nfc.common", 				"http://tizen.org/privacy/nfc"},
	{"http://tizen.org/privilege/nfc.p2p", 					"http://tizen.org/privacy/nfc"},
	{"http://tizen.org/privilege/nfc.tag", 					"http://tizen.org/privacy/nfc"},

	{"http://tizen.org/privilege/http", 					"http://tizen.org/privacy/internet"},
	{"http://tizen.org/privilege/socket", 					"http://tizen.org/privacy/internet"},
	{"http://tizen.org/privilege/web.service", 				"http://tizen.org/privacy/internet"},

	{"\0", "\0"}
};
*/

int
PrivacyIdInfo::initialize(void)
{
	static const std::string sqlPrivilege("SELECT PRIVILEGE_ID, PRIVACY_ID from PrivilegeToPrivacyTable");
	static const std::string sqlDeviceCap("SELECT DEVICE_CAP, PRIVACY_ID from DeviceCapToPrivacyTable");

	LOGI("enter");
	
	openDb(PRIVACY_INFO_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READONLY);
	prepareDb(pDbHandler, sqlPrivilege.c_str(), pStmtPrivilege);

	int res;
	while ( (res = sqlite3_step(pStmtPrivilege.get())) == SQLITE_ROW )
	{
		const char* privilegeId =  reinterpret_cast < const char* > (sqlite3_column_text(pStmtPrivilege.get(), 0));
		const char* privacyId =  reinterpret_cast < const char* > (sqlite3_column_text(pStmtPrivilege.get(), 1));
		m_privilegeToPrivacyMap.insert(std::map < std::string, std::string >::value_type(std::string(privilegeId), std::string(privacyId)));
		LOGD(" %s : %s", privilegeId, privacyId);
	}

	prepareDb(pDbHandler, sqlDeviceCap.c_str(), pStmtDeviceCap);
	while ( (res = sqlite3_step(pStmtDeviceCap.get())) == SQLITE_ROW )
	{
		const char* DeviceCap =  reinterpret_cast < const char* > (sqlite3_column_text(pStmtDeviceCap.get(), 0));
		const char* privacyId =  reinterpret_cast < const char* > (sqlite3_column_text(pStmtDeviceCap.get(), 1));
		m_deviceCapToPrivacyMap.insert(std::map < std::string, std::string >::value_type(std::string(DeviceCap), std::string(privacyId)));
	}
	m_isInitialized = true;

	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyIdInfo::getPrivacyIdFromPrivilege(const std::string privilege, std::string& privacyId)
{
	if (!m_isInitialized)
		initialize();
	std::map < std::string, std::string >::iterator iter = m_privilegeToPrivacyMap.find(privilege);
	if (iter == m_privilegeToPrivacyMap.end())
		return PRIV_MGR_ERROR_NO_DATA;
	privacyId =  iter->second;

	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyIdInfo::getPrivacyIdFromDeviceCap(const std::string deviceCap, std::string& privacyId)
{
	if (!m_isInitialized)
		initialize();
	std::map < std::string, std::string >::iterator iter = m_deviceCapToPrivacyMap.find(deviceCap);
	if (iter == m_deviceCapToPrivacyMap.end())
		return PRIV_MGR_ERROR_NO_DATA;
	privacyId =  iter->second;
	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyIdInfo::getPrivacyIdListFromPrivilegeList(const std::list < std::string> privilegeList, std::list < std::string> & privacyIdList)
{
	privacyIdList.clear();

	std::set <std::string> privacyIdSet;

	for (std::list < std::string > ::const_iterator iter = privilegeList.begin(); iter != privilegeList.end(); ++iter)
	{
		std::string privacyId;
		int res = getPrivacyIdFromPrivilege(*iter, privacyId);
		if (res == PRIV_MGR_ERROR_SUCCESS)
			privacyIdSet.insert(privacyId);
	}
	
	for (std::set < std::string >::iterator iter = privacyIdSet.begin(); iter != privacyIdSet.end(); ++iter)
	{
		privacyIdList.push_back(*iter);
	}

	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyIdInfo::getAllPrivacyId(std::list < std::string >& privacyIdList)
{
	static const std::string sql("SELECT PRIVACY_ID from PrivacyInfo");

	LOGI("enter");
	
	openDb(PRIVACY_INFO_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READONLY);
	prepareDb(pDbHandler, sql.c_str(), pStmt);

	int res;
	while ( (res = sqlite3_step(pStmt.get())) == SQLITE_ROW )
	{
		const char* privacyId =  reinterpret_cast < const char* > (sqlite3_column_text(pStmt.get(), 0));
		privacyIdList.push_back(std::string(privacyId));
		LOGD(" privacy Id : %s", privacyId);
	}

	return PRIV_MGR_ERROR_SUCCESS;
}