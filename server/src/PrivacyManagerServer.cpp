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
#include <PrivacyManagerServer.h>
#include <PrivacyManagerTypes.h>
#include <PrivacyDb.h>

std::mutex PrivacyManagerServer::m_singletonMutex;
PrivacyManagerServer* PrivacyManagerServer::m_pInstance = NULL;

void
PrivacyManagerServer::createDB(void)
{

}

int
PrivacyManagerServer::setPrivacySetting(const std::string pkgId, const std::string privacyId, bool enabled)
{
	int res = PrivacyDb::getInstance()->setPrivacySetting(pkgId, privacyId, enabled);
	TryReturn( res == PRIV_MGR_ERROR_SUCCESS, res, , "privacyDb::setPrivacySetting : %d", res);

	res = m_notificationServer.notifySettingChanged(pkgId, privacyId);
	TryReturn( res == PRIV_MGR_ERROR_SUCCESS, res, , "NotificationServer::notifySettingChanged : %d", res);

	return res;
}

int
PrivacyManagerServer::getPrivacyAppPackages(std::list <std::string>& list)
{
	return PrivacyDb::getInstance()->getPrivacyAppPackages(list);
}

int
PrivacyManagerServer::getAppPackagePrivacyInfo(const std::string pkgId, std::list < std::pair < std::string, bool > >& privacyInfoList)
{
	return PrivacyDb::getInstance()->getAppPackagePrivacyInfo(pkgId, privacyInfoList);
}


int
PrivacyManagerServer::addAppPackagePrivacyInfo(const std::string pkgId, const std::list < std::string > privilegeList, bool privacyPopupRequired)
{
	return PrivacyDb::getInstance()->addAppPackagePrivacyInfo(pkgId, privilegeList, privacyPopupRequired);
}

int
PrivacyManagerServer::removeAppPackagePrivacyInfo(const std::string pkgId)
{
	int res = PrivacyDb::getInstance()->removeAppPackagePrivacyInfo(pkgId);
	TryReturn( res == PRIV_MGR_ERROR_SUCCESS, res, , "privacyDb::removeAppPackagePrivacyInfo : %d", res);

	res = m_notificationServer.notifyPkgRemoved(pkgId);
	TryReturn( res == PRIV_MGR_ERROR_SUCCESS, res, , "NotificationServer::notifyPkgRemoved : %d", res);

	return res;
}

int
PrivacyManagerServer::isUserPrompted(const std::string pkgId, bool& isPrompted)
{
	return PrivacyDb::getInstance()->isUserPrompted(pkgId, isPrompted);
}

int
PrivacyManagerServer::setUserPrompted(const std::string pkgId, bool prompted)
{
	return PrivacyDb::getInstance()->setUserPrompted(pkgId, prompted);
}

PrivacyManagerServer::PrivacyManagerServer(void)
{

}

PrivacyManagerServer*
PrivacyManagerServer::getInstance(void)
{
	LOGI("enter");
	std::lock_guard < std::mutex > guard(m_singletonMutex);

	if (m_pInstance == NULL)
	{	
		m_pInstance = new PrivacyManagerServer();
		
		m_pInstance->m_notificationServer.initialize();
	}
	LOGI("leave");
	return m_pInstance;
}