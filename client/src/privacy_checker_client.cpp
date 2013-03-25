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
#include <PrivacyManager.h>
#include <privacy_manager_client.h>
#include <privacy_manager_client_types.h>
#include "privacy_manager_client_internal_types.h"
#include <PrivacyChecker.h>
#include <privacy_checker_client.h>
#include <privacy_info_client.h>
#include <PrivacyIdInfo.h>

int privacy_checker_initialize(const char *package_id)
{
	return PrivacyChecker::initialize(std::string(package_id));
}

int privacy_checker_check_privacy(const char *privacy_id)
{
	return PrivacyChecker::check(std::string(privacy_id));
}
int privacy_checker_check_privacy_by_privilege(const char *privilege_id)
{
	int res;
	std::string privacyId;

	res = PrivacyIdInfo::getPrivacyIdFromPrivilege(std::string(privilege_id), privacyId);
	if (res != PRIV_MGR_ERROR_SUCCESS)
		return res;

	return PrivacyChecker::check(privacyId);

}

int privacy_checker_check_privacy_by_device_cap(const char *device_cap)
{
	int res;
	std::string privacyId;

	res = PrivacyIdInfo::getPrivacyIdFromDeviceCap(std::string(device_cap), privacyId);
	if (res != PRIV_MGR_ERROR_SUCCESS)
		return res;

	return PrivacyChecker::check(privacyId);

}

int privacy_checker_finalize(void)
{
	return PrivacyChecker::finalize();
}
