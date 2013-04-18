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

#include "privacy-popup.h"

static appdata ad;
static service_h service;

static char *package_id = NULL;
static char *app_id = NULL;
static bundle *bundle_data = NULL;
int get_packageid_by_appid(const char* appid, char** packageid);
Evas_Object* create_conform(Evas_Object *parent);

static void destroy_privacy_popup()
{
	LOGD("exit");
	//elm_shutdown();
	elm_exit();
}

static void layout_cb(ui_gadget_h ug, enum ug_mode mode, void *priv) {
	LOGD("layout_cb");
    Evas_Object *base, *win;
    appdata *mydata;

    if (!ug || !priv)
        return;

    mydata = priv;

    base = ug_get_layout(ug);
    if (!base){
		LOGE("base layout from UG is null! %p", base);
        return;
	}

    win = ug_get_window();

    switch (mode) {
    case UG_MODE_FULLVIEW:
        evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(win, base);
        ug_disable_effect(ug);
        evas_object_show(base);
        break;
	default:
        break;
    }
}

static void result_cb(ui_gadget_h ug, service_h service, void *priv) {
    LOGD("result_cb");

    if (!ug || !priv)
        return;

	if(!service){
		LOGE("service is null. return without set_user_consented");
		return;
	}

	char* exit_value = NULL;
	service_get_extra_data(service, KEY_RESULT_EXIT, &exit_value);
	if(!exit_value){
		LOGD("no exit value from UG");
		return;
	}

	LOGD("exit value : %s", exit_value);

	if(strncmp(exit_value, KEY_RESULT_CANCEL, 2) == 0){
		int result = privacy_manager_client_set_user_consented(package_id, true);
		if(result != PRIV_MGR_ERROR_SUCCESS){
			LOGD("set_user_censented failed : %d", result);
		}
	}
}

static void destroy_cb(ui_gadget_h ug, void *priv) {
    LOGD("destroy_cb");

	int result = privacy_manager_client_set_user_consented(package_id, true);

	int res = aul_forward_app(app_id, bundle_data);
	if(res < 0){
		LOGE("aul_forward_app failed! result : %d", res);
		destroy_privacy_popup();
	}

    if (!ug || !priv)
        return;

	//ug_destroy(ug);
	//destroy_privacy_popup();
}

ui_gadget_h create_ug(appdata *data) {
	LOGD("create_ug");

    struct ug_cbs cbs = { 0, };
    cbs.layout_cb = layout_cb;
    cbs.result_cb = result_cb;
    cbs.destroy_cb = destroy_cb;
    cbs.priv = (void *) data;

	service_create(&service);
	LOGD("package_id : %s", package_id);
	service_set_operation(service, KEY_SERVICE_OPERATION);
	service_add_extra_data(service, KEY_PACKAGE_ID, package_id);

    data->ug = ug_create(NULL, "setting-privacy-efl", UG_MODE_FULLVIEW, service, &cbs);
	service_destroy(service);

	if(!data->ug){
		LOGE("ug_create failed!");
	}

    return data->ug;
}

static void on_done(void *data, Evas_Object *obj, void *event_info) {
    (void) data;
    (void) obj;
    (void) event_info;
	LOGD("on_done");
	destroy_privacy_popup();
}

Evas_Object* create_win(char* name)
{
	int w,h;
	Evas_Object *win = elm_win_add(NULL, PACKAGE, ELM_WIN_BASIC);
	if(!win){
		return NULL;
	}

	elm_win_title_set(win, PACKAGE);
	evas_object_smart_callback_add(win, "delete,request", on_done, NULL);
	ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
	evas_object_resize(win, w, h);
	return win;
}

Evas_Object* create_conform(Evas_Object *parent)
{
	Evas_Object *conform = elm_conformant_add(parent);
	if(!conform){
		return NULL;
	}
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, conform);
	evas_object_show(conform);

	return conform;
}

Evas_Object* create_layout(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(parent, layout);

	return layout;
}

EAPI_MAIN int elm_main() {
	LOGD("elm_main");

	Evas_Object *win = create_win(PACKAGE);
	if(!win){
		LOGE("failed to create window");
		return 0;
	}
	elm_win_conformant_set(win, EINA_TRUE);

	Evas_Object *conform = create_conform(win);
	if(!conform){
		LOGE("failed to create window");
		return 0;
	}

	// main layout
	Evas_Object *main_layout = create_layout(conform);
	if(!conform){
		LOGE("failed to create layout");
		return 0;
	}

    UG_INIT_EFL(win, UG_OPT_INDICATOR_ENABLE);
    evas_object_show(win);

	ad.win = win;
	ad.conform = conform;
	ad.main_layout = main_layout;

    return 0;
}

static int app_create(void *data)
{
	LOGD("app_create");
	return elm_main();
}

static int app_terminate(void *data)
{
	LOGD("app_terminate");

	free(package_id);
	free(app_id);
	bundle_free(bundle_data);
	return 0;
}

static int app_pause(void *data)
{
	LOGD("app_pause");

	destroy_privacy_popup();
	// else(ex screen of or lose top most)

	return 0;
}

static int app_resume(void *data)
{
	LOGD("app_resume");
	return 0;
}

static int app_reset(bundle *b, void *data)
{
	LOGD("app_reset");
	evas_object_show(ad.win);

	//AUL pass appid 
	const char *appid = bundle_get_val(b, "__AUL_PRIVACY_APPID__");

	if(!appid){
		LOGE("appid is null! ");
		destroy_privacy_popup();
		return 0;
	}

	app_id = strndup(appid, strlen(appid));
	LOGD("app id %s", appid);

	int result = get_packageid_by_appid(appid, &package_id);

	if(result != 0 || !package_id){
		LOGE("Failed to get package id! exit");
		destroy_privacy_popup();
	}

	LOGD("package id %s", package_id);
	bundle_data = bundle_dup(b);


	if(!create_ug(&ad)){
		destroy_privacy_popup();
	}
	return 0;
}

int main(int argc, char **argv) {
	elm_init(argc, argv);

	struct appcore_ops ops = {
		.create = app_create,
		.terminate = app_terminate,
		.pause = app_pause,
		.resume = app_resume,
		.reset = app_reset,
	};
	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}

int get_packageid_by_appid(const char* appid, char** packageid)
{
	int result = 0;
	char* pack_id = NULL;
	pkgmgrinfo_appinfo_h app_info_handle = NULL;

	result = pkgmgrinfo_appinfo_get_appinfo(appid, &app_info_handle);
	if(result != 0){
		LOGE("pkgmgrinfo_appinfo_get_appinfo() failed. result=[%d], appId=[%s]",result, appid);
		return -1;
	}

	result = pkgmgrinfo_appinfo_get_pkgname(app_info_handle, &pack_id);
	LOGD("pkgmgrinfo get_pkgname :%s", pack_id);

	if(pack_id){
		*packageid = (char*)malloc(sizeof(char)*strlen(pack_id));
		if(*packageid == NULL){
			LOGE("memory allocation failed to packageid!");
			return -1;
		}
		strncpy(*packageid, pack_id, strlen(pack_id));
	}
	else{
		LOGD("failed to get package id");
		return -1;
	}

	if(app_info_handle)
	{
		pkgmgrinfo_appinfo_destroy_appinfo(app_info_handle);
	}

	return 0;
}

