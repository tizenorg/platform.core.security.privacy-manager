/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <PrivacyChecker.h>
#include <PrivacyManager.h>
#include <SocketClient.h>
#include <algorithm> 
#include <memory>
#include <Utils.h>
#include <dlog.h>
#include <sqlite3.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <sys/types.h>
#include <unistd.h>

bool PrivacyChecker::m_isInitialized = false;
const std::string PrivacyChecker::DB_PATH("/opt/dbspace/.privacy.db");
std::map < std::string, bool >PrivacyChecker::m_privacyCache;
std::mutex PrivacyChecker::m_cacheMutex;
std::string PrivacyChecker::m_pkgId;
DBusConnection* PrivacyChecker::m_pDBusConnection;
GMainLoop* PrivacyChecker::m_pLoop = NULL;

const int MAX_LOCAL_BUF_SIZE = 128;

int
PrivacyChecker::getUniqueIdFromPackageId(const std::string pkgId, int& uniqueId)
{
	LOGI("enter");
	std::string PkgIdQuery = std::string("SELECT UNIQUE_ID from PackageInfo where PKG_ID=?");

	int res;
	openDb(DB_PATH.c_str(), pDbH, SQLITE_OPEN_READONLY);
	prepareDb(pDbH, PkgIdQuery.c_str(), pStmt);

	res = sqlite3_bind_text(pStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_IO_ERROR, , "sqlite3_bind_text : %d", res);

	res = sqlite3_step(pStmt.get());
	TryReturn( res == SQLITE_ROW, PRIV_MGR_ERROR_IO_ERROR, , "sqlite3_step : %d", res);

	uniqueId = sqlite3_column_int(pStmt.get(), 0);
	LOGI("%s : %d", pkgId.c_str(), uniqueId);

	LOGI("leave");
	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyChecker::initialize(const std::string pkgId)
{
	LOGI("enter");

	if (m_isInitialized)
		return PRIV_MGR_ERROR_SUCCESS;

	int res;
	pthread_t signalThread;
	res = pthread_create(&signalThread, NULL, &runSignalListenerThread, NULL);
	TryReturn(res >= 0, PRIV_MGR_ERROR_SYSTEM_ERROR, errno = res;, "Failed to create listener thread :%s", strerror(res));

	m_pkgId = pkgId;
	res = updateCache();
	TryReturn(res == 0, res, m_pkgId.clear(), "Failed to update cache : %d", res);

	m_isInitialized = true;

	LOGI("leave");

	return PRIV_MGR_ERROR_SUCCESS;
}

void*
PrivacyChecker::runSignalListenerThread(void* pData)
{
	pthread_detach(pthread_self());
	LOGI("Running g main loop for signal");
	
	if (m_pLoop != NULL)
		return (void*) 0;
	
	m_pLoop = g_main_new(TRUE);

	initializeDbus();

	g_main_loop_run(m_pLoop);

	finalizeDbus();

	pthread_exit(NULL);

	return (void*) 0;
}

int
PrivacyChecker::initializeDbus(void)
{
	LOGI("enter");
	DBusError error;
	dbus_error_init(&error);

	m_pDBusConnection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
	TryReturn(m_pDBusConnection != NULL, PRIV_MGR_ERROR_SYSTEM_ERROR, dbus_error_free(&error), "dbus_bus_get_private [%s] : %d", PRIV_MGR_ERROR_SYSTEM_ERROR);

	dbus_connection_setup_with_g_main(m_pDBusConnection, NULL);
	std::unique_ptr < char[] > pRule(new char[MAX_LOCAL_BUF_SIZE]);

	snprintf(pRule.get(), MAX_LOCAL_BUF_SIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH.c_str(), DBUS_SIGNAL_INTERFACE.c_str());
	dbus_bus_add_match(m_pDBusConnection, pRule.get(), &error);
	TryReturn(!dbus_error_is_set(&error), PRIV_MGR_ERROR_SYSTEM_ERROR, dbus_error_free(&error), "dbus_bus_add_match[%s] : %d", error.message, PRIV_MGR_ERROR_SYSTEM_ERROR);

	dbus_bool_t r = dbus_connection_add_filter(m_pDBusConnection, handleNotification, NULL, NULL);
	TryReturn(r, PRIV_MGR_ERROR_SYSTEM_ERROR, , "dbus_connection_add_filter: %d", PRIV_MGR_ERROR_SYSTEM_ERROR);

	LOGI("leave");

	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyChecker::finalizeDbus(void)
{
	dbus_connection_remove_filter(m_pDBusConnection, handleNotification, NULL);
	dbus_connection_close(m_pDBusConnection);
	m_pDBusConnection = NULL;

	return PRIV_MGR_ERROR_SUCCESS;
}


DBusHandlerResult
PrivacyChecker::handleNotification(DBusConnection* connection, DBusMessage* message, void* user_data)
{
	DBusError error;
	dbus_bool_t r;
	dbus_error_init(&error);

	char* pPkgId;
	char* pPrivacyId;

	if (dbus_message_is_signal(message, DBUS_SIGNAL_INTERFACE.c_str(), DBUS_SIGNAL_NAME.c_str()))
	{
		r = dbus_message_get_args(message, &error,
			DBUS_TYPE_STRING, &pPkgId,
			DBUS_TYPE_STRING, &pPrivacyId,
			DBUS_TYPE_INVALID);
		TryReturn(r, DBUS_HANDLER_RESULT_NOT_YET_HANDLED, , "Fail to get data : %s", error.message);
		
		if (std::string(pPkgId) == m_pkgId)
		{
			LOGI("Current app pkg privacy information updated");
			updateCache(pPrivacyId);
			printCache();
		}
	}
	else
	{
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
	
	return DBUS_HANDLER_RESULT_HANDLED;
}

int
PrivacyChecker::check(const std::string privacyId)
{
	LOGI("enter");

	std::lock_guard < std::mutex > guard(m_cacheMutex);

	TryReturn(m_isInitialized, -1, , "Not initialized");

	std::map < std::string, bool >::const_iterator iter = m_privacyCache.find(privacyId);
	if (iter == m_privacyCache.end() )
	{
		LOGD("NO matcheded");
		return -1;
	}
	else if (!iter->second)
	{
		LOGD("NOT allowed");
		return -1;
	}
	LOGD("OK");

	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyChecker::finalize(void)
{
	std::lock_guard <std::mutex> guard (m_cacheMutex);
	m_privacyCache.clear();
	
	if (m_pLoop != NULL)
	{
		g_main_loop_quit(m_pLoop);
		m_pLoop = NULL;
	}

	m_isInitialized = false;

	return PRIV_MGR_ERROR_SUCCESS;
}

int
PrivacyChecker::updateCache(void)
{
	LOGI("enter");
	static const std::string PrivacyQuery = "SELECT PRIVACY_ID, IS_ENABLED from Privacy where ID=?";
	
	std::lock_guard < std::mutex > guard(m_cacheMutex);

	int res;
	int id;
	res = getUniqueIdFromPackageId(m_pkgId, id);
	TryReturn( res == 0, -1, , "getUniqueIdFromPackageId : %d", res);
	LOGD("id : %d" ,id);
	
	openDb(DB_PATH.c_str(), pDbH, SQLITE_OPEN_READONLY);
	prepareDb(pDbH, PrivacyQuery.c_str(), pPrivacyStmt);
	res = sqlite3_bind_int(pPrivacyStmt.get(), 1, id);
	TryReturn( res == 0, -1, , "sqlite3_bind_int : %d", res);


	while ( sqlite3_step(pPrivacyStmt.get()) == SQLITE_ROW )
	{
		LOGI("start");
		const char* privacyId =  reinterpret_cast < const char* > (sqlite3_column_text(pPrivacyStmt.get(), 0));
		bool privacyEnabled = sqlite3_column_int(pPrivacyStmt.get(), 1) > 0 ? true : false;

		m_privacyCache.insert(std::map < std::string, bool >::value_type(std::string(privacyId), privacyEnabled));

		LOGD("Privacy found : %s %d", privacyId, privacyEnabled);
	}
	LOGI("leave");
	return PRIV_MGR_ERROR_SUCCESS;
}
void
PrivacyChecker::printCache(void)
{
	std::map < std::string, bool >::const_iterator iter = m_privacyCache.begin();
	for (; iter != m_privacyCache.end(); ++iter)
	{
		LOGD(" %s : %d", iter->first.c_str(), iter->second);
	}
}
int
PrivacyChecker::updateCache(const std::string privacyId)
{
	LOGI("enter");
	static const std::string PrivacyQuery = "SELECT IS_ENABLED from Privacy where ID=? and PRIVACY_ID=?";

	int res;
	int id;
	res = getUniqueIdFromPackageId(m_pkgId, id);
	TryReturn( res == 0, -1, , "getUniqueIdFromPackageId : %d", res);

	LOGD("id : %d" ,id);
	
	openDb(DB_PATH.c_str(), pDbH, SQLITE_OPEN_READONLY);
	prepareDb(pDbH, PrivacyQuery.c_str(), pPrivacyStmt);
	res = sqlite3_bind_int(pPrivacyStmt.get(), 1, id);
	TryReturn( res == 0, -1, , "sqlite3_bind_int : %d", res);

	res = sqlite3_bind_text(pPrivacyStmt.get(), 2, privacyId.c_str(),  -1, SQLITE_TRANSIENT);
	TryReturn( res == 0, -1, , "sqlite3_bind_text : %d", res);

	while ( sqlite3_step(pPrivacyStmt.get()) == SQLITE_ROW )
	{
		bool privacyEnabled = sqlite3_column_int(pPrivacyStmt.get(), 0) > 0 ? true : false;

		std::lock_guard < std::mutex > guard(m_cacheMutex);
		m_privacyCache.erase(privacyId);
		m_privacyCache.insert(std::map < std::string, bool >::value_type(privacyId, privacyEnabled));
	}
	
	LOGI("leave");

	return PRIV_MGR_ERROR_SUCCESS;
}