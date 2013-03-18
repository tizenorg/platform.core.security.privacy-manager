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

#ifndef _PRIVACY_INFO_H_
#define _PRIVACY_INFO_H_

#include <string>

struct PrivacyPrivilegePair
{
	char privilege[128];
	char privacy[128];

};

struct PrivacyPrivilegePair PrivilegeToPrivacyTable[] =
{
	{"http://tizendev.org/privilege/contact.read", "http://tizendev.org/privilege/contact"},
	{"http://tizendev.org/privilege/contact.write", "http://tizendev.org/privilege/contact"},
	{NULL, NULL}
};

class PrivacyInfo
{
public:
	static std::string GetPrivacyId(const std::string& privilegeId)
	{
		int i = 0;
		while (PrivilegeToPrivacyTable[i].privilege != NULL)		
		{
			if ( strncmp(privilegeId.c_str(), PrivilegeToPrivacyTable[i].privilege, privilegeId.size()) == 0)
				return std::string(PrivilegeToPrivacyTable[i].privacy);
		}
		return std::string("");
	}
}
#endif //_PRIVACY_INFO_H_