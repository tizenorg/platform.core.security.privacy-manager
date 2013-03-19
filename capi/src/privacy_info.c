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

#include <stdlib.h>
#include <privacy_manager.h>
#include <privacy_info.h>
#include <privacy_manager_client.h>
#include <privacy_manager_client_types.h>
#include <privacy_info_client.h>
#include <privacy_manager_types_private.h>

static int _privacy_info_convert_error(int error)
{

	return PRIVACY_MANAGER_ERROR_NONE;
}

int privacy_info_destroy(privacy_info_h privacy_info)
{
	int retval;
	if (privacy_info == NULL)
		return PRIV_MGR_ERROR_INVALID_PARAMETER;

	if (privacy_info->privacy_info_client)
	{
		retval = privacy_info_client_s_destroy(privacy_info->privacy_info_client);
		if (retval != PRIV_MGR_ERROR_SUCCESS)
			return retval;
	}

	free (privacy_info);

	return PRIV_MGR_ERROR_SUCCESS;
}

int privacy_info_get_privacy_id(privacy_info_h privacy_info, char **package)
{
	int retval = privacy_info_client_get_privacy_id(privacy_info->privacy_info_client, package);

	return _privacy_info_convert_error(retval);
}

int privacy_info_get_privacy_display_name(struct privacy_info_s* privacy_info, char **name)
{
	int retval = privacy_info_client_get_privacy_display_name(privacy_info->privacy_info_client, name);

	return  _privacy_info_convert_error(retval);
}

int privacy_info_get_privacy_description(struct privacy_info_s* privacy_info, char **description)
{
	int retval = privacy_info_client_get_privacy_description(privacy_info->privacy_info_client, description);

	return  _privacy_info_convert_error(retval);
}

int privacy_info_is_enabled(struct privacy_info_s* privacy_info, bool *enable)
{
	int retval = privacy_info_client_is_enabled(privacy_info->privacy_info_client, enable);

	return  _privacy_info_convert_error(retval);
}
