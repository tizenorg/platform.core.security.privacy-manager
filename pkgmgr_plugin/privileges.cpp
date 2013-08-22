//
// Open Service Platform
// Copyright (c) 2013 Samsung Electronics Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <errno.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <privacy_manager_client.h>
#include <dlog.h>
#include <list>
#include <string>

static const xmlChar _NODE_PRIVILEGES[]		= "privileges";
static const xmlChar _NODE_PRIVILEGE[]		= "privilege";
static const char TEST_AUTOMATION_PRIVILEGE[] = "http://tizen.org/privilege/testautomation";

void destroy_char_list(char** ppList, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		if (ppList[i])
			free(ppList[i]);
	}
	free(ppList);
}

extern "C"
__attribute__ ((visibility("default")))
int PKGMGR_PARSER_PLUGIN_INSTALL(xmlDocPtr docPtr, const char* packageId)
{
	int ret;
    bool privacyPopupRequired = true;

	// Node: <privileges>
	xmlNodePtr curPtr = xmlFirstElementChild(xmlDocGetRootElement(docPtr));

	curPtr = curPtr->xmlChildrenNode;
	if (curPtr == NULL)
	{
		LOGD("No privileges");
		return 0;
	}

	std::list <std::string> privilegeList;
	while (curPtr != NULL)
	{
		if (xmlStrcmp(curPtr->name, _NODE_PRIVILEGE) == 0)
		{
			xmlChar* pPrivilege = xmlNodeListGetString(docPtr, curPtr->xmlChildrenNode, 1);
            
			if (pPrivilege == NULL)
			{
				LOGE("Failed to get value");
				return -EINVAL;
			}
            if (strncmp(reinterpret_cast<char*>(pPrivilege), TEST_AUTOMATION_PRIVILEGE, strlen(TEST_AUTOMATION_PRIVILEGE) ) == 0 )
            {
            	SECURE_LOGD("No privacy popup");
                privacyPopupRequired = false;
            }
            else 
			{
				privilegeList.push_back(std::string( reinterpret_cast<char*> (pPrivilege)));
			}
		}
		curPtr = curPtr->next;
	}

	char** ppPrivilegeList = (char**) calloc(privilegeList.size() + 1, sizeof(char*));
	std::list <std::string>::iterator iter = privilegeList.begin();
	for (int i = 0; i < privilegeList.size(); ++i)
	{
		ppPrivilegeList[i] = (char*)calloc (strlen(iter->c_str()) + 1, sizeof(char));
		if (ppPrivilegeList[i] == NULL)
		{
			destroy_char_list(ppPrivilegeList, privilegeList.size() + 1);
			return -ENOMEM;
		}
		memcpy(ppPrivilegeList[i], iter->c_str(), strlen(iter->c_str()));
		++iter;
	}

	ppPrivilegeList[privilegeList.size()] = (char*)calloc (2, sizeof(char));
	memcpy(ppPrivilegeList[privilegeList.size()], "\0", 1);

	ret = privacy_manager_client_install_privacy(packageId, (const char**) ppPrivilegeList, privacyPopupRequired);
	destroy_char_list(ppPrivilegeList, privilegeList.size() + 1);
	if (ret != PRIV_MGR_ERROR_SUCCESS)
	{
		LOGD("Failed to install privacy info: %d", ret);
		return -EINVAL;
	}

    return 0;
}

extern "C"
__attribute__ ((visibility("default")))
int PKGMGR_PARSER_PLUGIN_UNINSTALL(xmlDocPtr docPtr, const char* packageId)
{
	int res = privacy_manager_client_uninstall_privacy_by_server(packageId);
	if (res != PRIV_MGR_ERROR_SUCCESS)
	{
		LOGD("Failed to uninstall privacy info in server: %d", res);
		
		res = privacy_manager_client_uninstall_privacy(packageId);
		if (res != PRIV_MGR_ERROR_SUCCESS)
		{
			LOGD("Failed to uninstall privacy info: %d", res);
			return -EINVAL;
		}
	}

	return 0;
}

extern "C"
__attribute__ ((visibility("default")))
int PKGMGR_PARSER_PLUGIN_UPGRADE(xmlDocPtr docPtr, const char* packageId)
{
	int res = 0;
    
    LOGD("Update privacy Info");

	res = PKGMGR_PARSER_PLUGIN_UNINSTALL(docPtr, packageId);
	if (res != 0)
	{
		LOGD("Privacy info can be already uninstalled");
	}

	res = PKGMGR_PARSER_PLUGIN_INSTALL(docPtr, packageId);
	if (res != 0)
	{
		LOGD("Failed to install privacy Info: %d", res);
	}
	return res;
}
