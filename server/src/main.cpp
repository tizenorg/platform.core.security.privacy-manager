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

#include <PrivacyManagerDaemon.h>
#include <glib.h>

int
main(int argc, char* argv[])
{
	PrivacyManagerDaemon* pDaemon = PrivacyManagerDaemon::getInstance();

	pDaemon->inialize();
	pDaemon->start();

	GMainLoop* pLoop;
	pLoop = g_main_new(TRUE);

	g_main_loop_run(pLoop);

	pDaemon->stop();
	pDaemon->shutdown();

	return 0;
}