/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */

#include <string.h>
#include <string>
#include <memory>
#include <PrivacyManagerClient.h>
#include <privacy_manager_client.h>
#include <privacy_manager_client_types.h>
#include "privacy_manager_client_internal_types.h"

int privacy_info_client_s_destroy(privacy_info_client_s* privacy_info)
{
	if (privacy_info != NULL)
	{
		if (privacy_info->privacy_id)
			free(privacy_info->privacy_id);
		free (privacy_info);
	}

	return PRIV_MGR_ERROR_SUCCESS;
}

int privacy_info_client_get_privacy_id(privacy_info_client_s* privacy_info, char **privacy_id)
{
	int size = strlen(privacy_info->privacy_id);
	*privacy_id = (char*) calloc(1, size + 1);

	memcpy (*privacy_id, privacy_info->privacy_id, size);

	return PRIV_MGR_ERROR_SUCCESS;
}

int privacy_info_client_get_privacy_display_name(privacy_info_client_s* privacy_info, char **name)
{
	// TODO: Modify to use STMS
	std::string temp = std::string(privacy_info->privacy_id);
	int startIndex = temp.find_last_of("/");
	std::string displayName = temp.substr(startIndex + 1, temp.size());

	int size = strlen(displayName.c_str());
	*name = (char*) calloc(1, size + 1);

	memcpy (*name, displayName.c_str(), size);

	return PRIV_MGR_ERROR_SUCCESS;
}

int privacy_info_client_get_privacy_description(privacy_info_client_s* privacy_info, char **description)
{
	// TODO: Modify to use STMS
	char tempDesc[] = "Undefined";
	int size = strlen(tempDesc);
	*description = (char*) calloc(1, size + 1);

	memcpy (*description, tempDesc, size);

	return PRIV_MGR_ERROR_SUCCESS;
}

int privacy_info_client_is_enabled(privacy_info_client_s* privacy_info, bool *enabled)
{
	*enabled = privacy_info->is_enabled;
	return PRIV_MGR_ERROR_SUCCESS;
}
