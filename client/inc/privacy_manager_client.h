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

#ifndef __TIZEN_PRIVACYMGR_PRIVACY_MANAGER_CLIENT_H
#define __TIZEN_PRIVACYMGR_PRIVACY_MANAGER_CLIENT_H

#include <privacy_info_client.h>
#include <privacy_manager_client_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*privacy_manager_client_privacy_packages_cb) (const char *package_id, void* user_data);
typedef bool (*privacy_manager_client_privacy_info_cb) (privacy_info_client_s* privacy_info, void* user_data);

EXTERN_API int privacy_manager_client_install_privacy(const char *package_id, const char** privacy_list);
EXTERN_API int privacy_manager_client_uninstall_privacy(const char *package_id);
EXTERN_API int privacy_manager_client_foreach_privacy_packages(privacy_manager_client_privacy_packages_cb callback, void *user_data);
EXTERN_API int privacy_manager_client_foreach_get_privacy_info(const char *package_id, privacy_manager_client_privacy_info_cb callback, void* user_data);
EXTERN_API int privacy_manager_client_set_package_privacy(const char *package_id, const char *privacy_id, bool enable);


#ifdef __cplusplus
}
#endif


#endif //__TIZEN_PRIVACYMGR_PRIVACY_MANAGER_CLIENT_H

