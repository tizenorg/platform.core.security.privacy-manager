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

#ifndef _PRIVACY_DB_H_
#define _PRIVACY_DB_H_

#include <string>
#include <memory>
#include <list>
#include <mutex>
#include <sqlite3.h>
#include <NotificationServer.h>

class NotificationServer;
struct sqlite3;
class a;
class PrivacyManagerServer
{
private:
	static std::mutex m_singletonMutex;
	static const std::string DB_PATH;
	static PrivacyManagerServer* m_pInstance;
	sqlite3* m_pDBHandler;
	NotificationServer m_notificationServer;

	const static std::string CREATE_PACKAGE_INFO_TABLE;
	const static std::string CREATE_PRIVACY_TABLE;

private:
	void createDB(void);
	int isPackageIdAreadyExisted(const std::string pkgId, bool& isExisted);

public:

	explicit PrivacyManagerServer(void);

	static PrivacyManagerServer* getInstance(void);

	sqlite3* getDBHandler(void);

	int getUniqueIdFromPackageId(const std::string pkgId, int& id);

	int getPrivacyAppPackages(std::list <std::string>& list);

	int getAppPackagePrivacyInfo(const std::string pkgId, std::list < std::pair < std::string, bool > > *pList);

	int setPrivacySetting(const std::string pkgId, const std::string privacyId, bool enabled);

	int addAppPackagePrivacyInfo(const std::string pkgcId, const std::list < std::string > privilegeList);

	int removeAppPackagePrivacyInfo(const std::string pkgId);

	int isUserPrompted(const std::string pkgId, bool& isPrompted);

	int setUserPrompted(const std::string pkgId, bool prompted);
};


#endif // _PRIVACY_DB_H_