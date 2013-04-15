
/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <Elementary.h>
#include <Ecore_X.h>
#include "dlog.h"
#include <Evas.h>
#include <app.h>


#include <pkgmgr-info.h>
#include <aul.h>
#include <bundle.h>
#include <appcore-efl.h>
#include <privacy_manager_client.h>

#include <stdio.h>
#include <ui-gadget.h>
#include <ui-gadget-module.h>
#include <ui-gadget-engine.h>


#ifdef LOG_TAG
    #undef LOG_TAG
#endif

#ifndef LOG_TAG
    #define LOG_TAG "PRIVACY_POPUP"
#endif

#define PACKAGE "privacy-popup"

typedef struct appdata_t {
    ui_gadget_h ug;
	Evas_Object *conform;
    Evas_Object *main_layout;
	Evas_Object *win;
} appdata;

const char *KEY_PACKAGE_ID = "pkg_id";
const char *KEY_SERVICE_OPERATION = "http://tizen.org/appcontrol/operation/configure/privacy/service";
const char *KEY_RESULT_EXIT = "exit";
const char *KEY_RESULT_OK = "ok";
const char *KEY_RESULT_CANCEL = "cancel";
