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

#include <sstream>
#include <dlog.h>
#include <Utils.h>
#include <PrivacyDb.h>
#include <PrivacyManagerTypes.h>
#include <sqlite3.h>
#include <pkgmgr-info.h>

std::mutex PrivacyDb::m_singletonMutex;
PrivacyDb* PrivacyDb::m_pInstance = NULL;

void
PrivacyDb::createDB(void)
{

}

bool PrivacyDb::isFilteredPackage(const std::string pkgId) const
{
	pkgmgrinfo_pkginfo_h handle;

	int res = pkgmgrinfo_pkginfo_get_pkginfo(pkgId.c_str(), &handle);

	if (res != PMINFO_R_OK)
		return false;

	bool preloaded = false;
	res = pkgmgrinfo_pkginfo_is_preload(handle, &preloaded);
	pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

	return preloaded;
}

int
PrivacyDb::setPrivacySetting(const std::string pkgId, const std::string privacyId, bool enabled)
{
	LOGI("enter");

	static const std::string query = std::string("UPDATE PrivacyInfo set IS_ENABLED =? where PKG_ID=? and PRIVACY_ID=?");

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READWRITE);
	prepareDb(pDbHandler, query.c_str(), pStmt);

	int res = sqlite3_bind_int(pStmt.get(), 1, enabled);
	TryReturn(res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

	res = sqlite3_bind_text(pStmt.get(), 2, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn(res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

	res = sqlite3_bind_text(pStmt.get(), 3, privacyId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);
	
	res = sqlite3_step(pStmt.get());
	TryReturn( res == SQLITE_DONE, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);

	LOGI("leave");

	return 0;
}

int
PrivacyDb::getPrivacyAppPackages(std::list <std::string>& list) const
{
	LOGI("enter");

	std::string query = "SELECT PKG_ID from PackageInfo";

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READONLY);
	prepareDb(pDbHandler, query.c_str(), pStmt);

	while ( sqlite3_step(pStmt.get()) == SQLITE_ROW )
	{
		const char* pValue =  reinterpret_cast < const char* > (sqlite3_column_text(pStmt.get(), 0));

		LOGD("PkgId found : %s ", pValue);
		std::string pkgId = std::string(pValue);

		if (isFilteredPackage(pkgId))
		{
			LOGD("%s is Filtered", pValue);
			continue;
		}
		list.push_back(std::string(pValue));
	}

	LOGI("leave");

	return 0;
}

int
PrivacyDb::getAppPackagePrivacyInfo(const std::string pkgId, std::list < std::pair < std::string, bool > >& privacyInfoList) const
{
	LOGI("enter");

	static const std::string query = "SELECT PRIVACY_ID, IS_ENABLED from PrivacyInfo where PKG_ID=?";

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READONLY);
	prepareDb(pDbHandler, query.c_str(), pStmt);

	int res = sqlite3_bind_text(pStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

	while ( (res= sqlite3_step(pStmt.get())) == SQLITE_ROW )
	{
		const char* privacyId =  reinterpret_cast < const char* > (sqlite3_column_text(pStmt.get(), 0));
		bool privacyEnabled = sqlite3_column_int(pStmt.get(), 1) > 0 ? true : false;

		privacyInfoList.push_back( std::pair <std::string, bool> (std::string(privacyId), privacyEnabled) );

		LOGD("Privacy found : %s %d", privacyId, privacyEnabled);
	}

	LOGI("leave");

	return 0;
}


int
PrivacyDb::addAppPackagePrivacyInfo(const std::string pkgId, const std::list < std::string > privilegeList)
{
	LOGI("enter");

	static const std::string pkgInfoQuery("INSERT INTO PackageInfo(PKG_ID, IS_SET) VALUES(?, ?)");
	static const std::string privacyQuery("INSERT INTO PrivacyInfo(PKG_ID, PRIVACY_ID, IS_ENABLED) VALUES(?, ?, ?)");
	
	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READWRITE);
	prepareDb(pDbHandler, pkgInfoQuery.c_str(), pPkgInfoStmt);

	int res = sqlite3_bind_text(pPkgInfoStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);
	
	res = sqlite3_bind_int(pPkgInfoStmt.get(), 2, 0);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

	res = sqlite3_step(pPkgInfoStmt.get());
	TryReturn( res == SQLITE_DONE, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);
	
	for ( std::list <std::string>::const_iterator iter = privilegeList.begin(); iter != privilegeList.end(); ++iter)
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
}

int
PrivacyDb::removeAppPackagePrivacyInfo(const std::string pkgId)
{
	LOGI("enter");

	static const std::string pkgInfoQuery("DELETE FROM PackageInfo WHERE PKG_ID=?");
	static const std::string privacyQuery("DELETE FROM PrivacyInfo WHERE PKG_ID=?");

	int res;

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READWRITE);
	prepareDb(pDbHandler, pkgInfoQuery.c_str(), pPkgInfoStmt);

	res = sqlite3_bind_text(pPkgInfoStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);

	res = sqlite3_step(pPkgInfoStmt.get());
	TryReturn( res == SQLITE_DONE, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);

	prepareDb(pDbHandler, privacyQuery.c_str(), pPrivacyStmt);

	res = sqlite3_bind_text(pPrivacyStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);	

	res = sqlite3_step(pPrivacyStmt.get());
	TryReturn( res == SQLITE_DONE, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);

	return 0;
}

int
PrivacyDb::isUserPrompted(const std::string pkgId, bool& isPrompted) const
{
	LOGI("enter");

	static const std::string query = "SELECT IS_SET from PackageInfo where PKG_ID=?";

	isPrompted = true;

	if (isFilteredPackage(pkgId))
	{
		LOGD("%s is Filtered", pkgId.c_str());
		return 0;
	}

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READONLY);
	prepareDb(pDbHandler, query.c_str(), pStmt);

	int res = sqlite3_bind_text(pStmt.get(), 1, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);

	std::unique_ptr < std::list < std::pair < std:: string, bool > > > pList ( new std::list < std::pair < std:: string, bool > >);

	if ((res = sqlite3_step(pStmt.get())) == SQLITE_ROW)
	{
		isPrompted = sqlite3_column_int(pStmt.get(), 0) > 0 ? true : false;
	}
	else
	{
		LOGE("The package[%s] can not access privacy", pkgId.c_str());
		return PRIV_MGR_ERROR_SUCCESS;
	}

	return 0;
}

int
PrivacyDb::setUserPrompted(const std::string pkgId, bool prompted)
{
	LOGI("enter");

	std::string query = std::string("UPDATE PackageInfo set IS_SET =? where PKG_ID=?");

	int res;

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READWRITE);
	prepareDb(pDbHandler, query.c_str(), pStmt);

	res = sqlite3_bind_int(pStmt.get(), 1, prompted? 1 : 0);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_int : %d", res);

	res = sqlite3_bind_text(pStmt.get(), 2, pkgId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);

	res = sqlite3_step(pStmt.get());
	TryReturn( res == SQLITE_DONE, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_step : %d", res);

	LOGI("leave");

	return 0;
}

int
PrivacyDb::getAppPackagesbyPrivacyId(std::string privacyId, std::list < std::pair < std::string, bool > >& list) const
{
	LOGI("enter");

	std::string sql = std::string("SELECT PKG_ID, IS_ENABLED from PrivacyInfo where PRIVACY_ID=?");

	openDb(PRIVACY_DB_PATH.c_str(), pDbHandler, SQLITE_OPEN_READWRITE);
	prepareDb(pDbHandler, sql.c_str(), pStmt);

	LOGD("privacy id : %s", privacyId.c_str());
	int res = sqlite3_bind_text(pStmt.get(), 1, privacyId.c_str(), -1, SQLITE_TRANSIENT);
	TryReturn( res == SQLITE_OK, PRIV_MGR_ERROR_DB_ERROR, , "sqlite3_bind_text : %d", res);

	while ( sqlite3_step(pStmt.get()) == SQLITE_ROW )
	{
		
		const char* pPkgId =  reinterpret_cast < const char* > (sqlite3_column_text(pStmt.get(), 0));
		bool isEnabled = sqlite3_column_int(pStmt.get(), 1) > 0 ? true : false;
        std::string pkgId = std::string(pPkgId);
		if (isFilteredPackage(pkgId))
		{
			LOGD("%s is Filtered", pPkgId);
			continue;
		}

		list.push_back( std::pair <std::string, bool >(pkgId, isEnabled) );
	}

	LOGI("leave");

	return PRIV_MGR_ERROR_SUCCESS;
}

PrivacyDb::PrivacyDb(void)
{

}

PrivacyDb::~PrivacyDb(void)
{

}

PrivacyDb*
PrivacyDb::getInstance(void)
{
	LOGI("enter");
	std::lock_guard < std::mutex > guard(m_singletonMutex);

	if (m_pInstance == NULL)
	{	
		m_pInstance = new PrivacyDb();
	}

	LOGI("leave");

	return m_pInstance;
}
