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


#include <TestService.h>

void
TestService::test(SocketConnection* pConnector)
{
	std::string res1, res2;
	pConnector->read(&res1, &res2);
	LOGD("TEST received: %s %s", res1.c_str(),res2.c_str());

	pConnector->write(std::string("-test OK-"));
	pConnector->write(std::string("-byebye-"));
}
