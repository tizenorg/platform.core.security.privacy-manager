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

#ifndef _PRIVACY_ID_INFO_H_
#define _PRIVACY_ID_INFO_H_

#include <string>
#include <map>
#include <list>

class PrivacyIdInfo
{
private:
	static std::map <std::string, std::string> m_privilegeToPrivacyMap;
	static bool m_isInitialized;

public:
	static int initialize(void);
	static int getPrivacyIdFromPrivilege(const std::string privilege, std::string& privacyId);
	static int getPrivacyIdListFromPrivilegeList(const std::list < std::string> privilegeList, std::list < std::string> & privacyIdList);
	static int getAllPrivacyId(std::list < std::string >& privacyIdList);

	static int getPrivaycDisplayName(const std::string privacyId, std::string& displayName);
	static int getPrivaycDescription(const std::string privacyId, std::string& description);
};

#endif //_PRIVACY_ID_INFO_H_