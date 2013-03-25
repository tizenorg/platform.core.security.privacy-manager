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

#include <PrivacyManager.h>
#include <SocketClient.h>
#include <PrivacyIdInfo.h>
#include <algorithm> 
#include <memory>
#include <Utils.h>

std::mutex PrivacyManager::m_singletonMutex;
PrivacyManager* PrivacyManager::m_pInstance = NULL;
const std::string PrivacyManager::INTERFACE_NAME("PrivacyInfoService");

PrivacyManager::PrivacyManager(void)
{
	std::unique_ptr<SocketClient> pSocketClient(new SocketClient(INTERFACE_NAME));
	m_pSocketClient = std::move(pSocketClient);
}

PrivacyManager*
PrivacyManager::getInstance(void)
{
	std::lock_guard<std::mutex> guard(m_singletonMutex);
	if (m_pInstance == NULL)
		m_pInstance = new PrivacyManager();
	return m_pInstance;
}

int
PrivacyManager::addAppPackagePrivacyInfo(const std::string pkgId, const std::list < std::string >& pList)
{
	int result;

	std::list < std::string > privacyList;

	result = PrivacyIdInfo::getPrivacyIdListFromPrivilegeList(pList, privacyList);
	if (result != PRIV_MGR_ERROR_SUCCESS )
		return result;

	if (privacyList.size() == 0)
		return PRIV_MGR_ERROR_SUCCESS;

	m_pSocketClient->connect();
	m_pSocketClient->call("addPrivacyInfo", pkgId, privacyList, &result);
	m_pSocketClient->disconnect();

	return result;
}

int
PrivacyManager::removeAppPackagePrivacyInfo(const std::string pkgId)
{
	int result;
	m_pSocketClient->connect();
	m_pSocketClient->call("removePrivacyInfo", pkgId, pkgId, &result);
	m_pSocketClient->disconnect();

	return result;
}

int
PrivacyManager::setPrivacySetting(const std::string pkgId, const std::string privacyId, bool isEnabled)
{
	int result;
	m_pSocketClient->connect();
	m_pSocketClient->call("setPrivacySetting", pkgId, privacyId, isEnabled, &result);
	m_pSocketClient->disconnect();

	return result;
}
	
int
PrivacyManager::getPrivacyAppPackages(std::list < std::string >& pList)
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
PrivacyManager::getAppPackagePrivacyInfo(const std::string pkgId, std::list < std::pair <std::string, bool > > & list)
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
PrivacyManager::isUserPrompted(const std::string pkgId, bool& isPrompted)
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
PrivacyManager::setUserPrompted(const std::string pkgId, bool prompted)
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