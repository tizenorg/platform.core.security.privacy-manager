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

#ifndef _PRIVACY_MANAGER_DAEMON_H_
#define _PRIVACY_MANAGER_DAEMON_H_

#include <privacy_manager_client_types.h>

class SocketService;

class EXTERN_API PrivacyManagerDaemon
{
private:
	static PrivacyManagerDaemon* pInstance;
	SocketService* pSocketService;

private:
	PrivacyManagerDaemon(void);
	~PrivacyManagerDaemon(void);


public:
	static PrivacyManagerDaemon* getInstance(void);
	int initialize(void);
	int start(void);
	int stop(void);
	int shutdown(void);
};

#endif // _PRIVACY_MANAGER_DAEMON_H_
