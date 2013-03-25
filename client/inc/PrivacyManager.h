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

#ifndef _PRIVACY_MANAGER_H_
#define _PRIVACY_MANAGER_H_

#include <string>
#include <mutex>
#include <list>
#include <vector>
#include <memory>
#include <PrivacyManagerTypes.h>

class SocketClient;

class EXTERN_API PrivacyManager
{
private:
	static PrivacyManager* m_pInstance;
	static const std::string INTERFACE_NAME;

	std::unique_ptr< SocketClient > m_pSocketClient;

	static std::mutex m_singletonMutex;

	PrivacyManager();
	~PrivacyManager();

public:
	static PrivacyManager* getInstance(void);

	int addAppPackagePrivacyInfo(const std::string pkgId, const std::list < std::string >& list);

	int removeAppPackagePrivacyInfo(const std::string pkgId);

	int setPrivacySetting(const std::string pkgId, const std::string privacyId, bool isEnabled);
	
	int getPrivacyAppPackages(std::list < std::string >& list);

	int	getAppPackagePrivacyInfo(const std::string pkgId, std::list < std::pair <std::string, bool > > & list);

	int isUserPrompted(const std::string pkgId, bool& isPrompted);

	int setUserPrompted(const std::string pkgId, bool prompted);
};

#endif // _PRIVACY_MANAGER_H_