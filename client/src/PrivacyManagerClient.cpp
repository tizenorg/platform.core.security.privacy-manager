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

#include <PrivacyManagerClient.h>
#include <SocketClient.h>
#include <PrivacyIdInfo.h>
#include <algorithm> 
#include <memory>
#include <Utils.h>

std::mutex PrivacyManagerClient::m_singletonMutex;
PrivacyManagerClient* PrivacyManagerClient::m_pInstance = NULL;
const std::string PrivacyManagerClient::INTERFACE_NAME("PrivacyInfoService");

PrivacyManagerClient::PrivacyManagerClient(void)
{
	std::unique_ptr<SocketClient> pSocketClient(new SocketClient(INTERFACE_NAME));
	m_pSocketClient = std::move(pSocketClient);
}

PrivacyManagerClient*
PrivacyManagerClient::getInstance(void)
{
	std::lock_guard<std::mutex> guard(m_singletonMutex);
	if (m_pInstance == NULL)
		m_pInstance = new PrivacyManagerClient();
	return m_pInstance;
}

int
PrivacyManagerClient::addAppPackagePrivacyInfo(const std::string pkgId, const std::list < std::string >& pList)
{

	std::list < std::string > privacyList;

	int res = PrivacyIdInfo::getPrivacyIdListFromPrivilegeList(pList, privacyList);
	if (res != PRIV_MGR_ERROR_SUCCESS )
		return res;

	if (privacyList.size() == 0)
		return PRIV_MGR_ERROR_SUCCESS;

#ifdef __READ_DB_IPC__
	m_pSocketClient->connect();
	m_pSocketClient->call("addPrivacyInfo", pkgId, privacyList, &result);
	m_pSocketClient->disconnect();

	return result;
#else
	LOGI("enter");

	static const std::string pkgInfoQuery("INSERT INTO PackageInfo(PKG_ID, IS_SET) VALUES(?, ?)");
	static const std::string privacyQuery("INSERT INTO PrivacyInfo(PKG_ID, PRIVACY_ID, IS_ENABLED) VALUES(?, ?, ?)");
	
	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READWRITE);
	prepareDb(pDbHandler, pkgInfoQuery.c_str(), pPkgInfoStmt);

	res = sqlite3_bind_text(pPkgInfoStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);
	
	res = sqlite3_bind_int(pPkgInfoStmt.get(), 2, 0);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

	res = sqlite3_step(pPkgInfoStmt.get());
	TryReturn( res == SQLITE_DONE || res == SQLITE_CONSTRAINT, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);
	
	for ( std::list <std::string>::const_iterator iter = privacyList.begin(); iter != privacyList.end(); ++iter)
	{
		LOGD(" install privacy: %s", iter->c_str());
		prepareDb(pDbHandler, privacyQuery.c_str(), pPrivacyStmt);
		
		res = sqlite3_bind_text(pPrivacyStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
		TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

		res = sqlite3_bind_text(pPrivacyStmt.get(), 2, iter->c_str(), -1, SQLITE_TRANSIENT);
		TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);
		
		// Before setting app and popup is ready, default value is true
		res = sqlite3_bind_int(pPrivacyStmt.get(), 3, 1);
		TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

		res = sqlite3_step(pPrivacyStmt.get());
		TryReturn( res == SQLITE_DONE || res == SQLITE_CONSTRAINT, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);

		sqlite3_reset(pPrivacyStmt.get());
	}

	return 0;
#endif
}

int
PrivacyManagerClient::removeAppPackagePrivacyInfo(const std::string pkgId)
{
	int result;
	m_pSocketClient->connect();
	m_pSocketClient->call("removePrivacyInfo", pkgId, &result);
	m_pSocketClient->disconnect();

	return result;
}

int
PrivacyManagerClient::setPrivacySetting(const std::string pkgId, const std::string privacyId, bool isEnabled)
{
	int result;
	m_pSocketClient->connect();
	m_pSocketClient->call("setPrivacySetting", pkgId, privacyId, isEnabled, &result);
	m_pSocketClient->disconnect();

	return result;
}
	
int
PrivacyManagerClient::getPrivacyAppPackages(std::list < std::string >& pList)
{
	int result, size;
	std::string temp1;
	SocketClient* p = new SocketClient(INTERFACE_NAME);
	p->connect();
	p->call("getPrivacyAppPackages", &result, &size, &pList);
	p->disconnect();

	return result;
}

int
PrivacyManagerClient::getAppPackagePrivacyInfo(const std::string pkgId, std::list < std::pair <std::string, bool > > & list)
{

	std::unique_ptr <SocketClient> pSocketClient (new SocketClient(INTERFACE_NAME));

	int result;
	pSocketClient->connect();
	pSocketClient->call("getAppPackagePrivacyInfo", pkgId, &result, &list);

	pSocketClient->disconnect();

	for (std::list < std::pair <std::string, bool > >::const_iterator iter = list.begin(); iter != list.end(); ++iter){
		LOGD(" %s : %d", iter->first.c_str(), iter->second);
	}

	return result;
}

int
PrivacyManagerClient::isUserPrompted(const std::string pkgId, bool& isPrompted)
{
	LOGI("enter");

	std::unique_ptr <SocketClient> pSocketClient (new SocketClient(INTERFACE_NAME));

	int result;
	pSocketClient->connect();
	pSocketClient->call("isUserPrompted", pkgId, &result, &isPrompted);
	pSocketClient->disconnect();

	LOGI("leave");

	return result;
}

int
PrivacyManagerClient::setUserPrompted(const std::string pkgId, bool prompted)
{
	LOGI("enter");

	std::unique_ptr <SocketClient> pSocketClient (new SocketClient(INTERFACE_NAME));

	int result;
	pSocketClient->connect();
	pSocketClient->call("setUserPrompted", pkgId, prompted, &result);
	pSocketClient->disconnect();
	LOGI("leave");

	return result;
}