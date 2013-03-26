

#include <iostream>
#include <SocketClient.h>
#include <PrivacyManagerClient.h>
#include <PrivacyChecker.h>
#include <glib.h>
#include <privacy_manager_client.h>

bool __privacy_manager_client_privacy_info_cb(privacy_info_client_s* privacy_info, void* user_data)
{
	std::cout << "-----result : " << *(int*) user_data << std::endl;

	char* name, *id, *desc;
	bool enabled;
	int res;
	res = privacy_info_client_get_privacy_id(privacy_info, &id);
	std::cout << " id:      " << id << " result:" << res << std::endl;

	res = privacy_info_client_get_privacy_display_name(privacy_info, &name);
	std::cout << " name:    " << name << " result:" << res << std::endl;

	res = privacy_info_client_get_privacy_description(privacy_info, &desc);
	std::cout << " desc:    " << desc << " result:" << res << std::endl;

	res = privacy_info_client_is_enabled(privacy_info, &enabled);
	std::cout << " enabled: " << enabled << " result:" << res << std::endl;

	return true;
}

bool __privacy_manager_client_privacy_packages_cb(const char *package_id, void* user_data)
{
	int i = *(int*)user_data;
	std::cout << "__privacy_manager_client_privacy_packages_cb: userdata: " << i++ << " : " << package_id <<std::endl;

	int res = privacy_manager_client_foreach_get_privacy_info(package_id, __privacy_manager_client_privacy_info_cb, user_data);
	std::cout << "__privacy_manager_client_privacy_packages_cb: result:" << res <<std::endl;

	return true;
}

int check_addAppPackagePrivacyInfo(void)
{
	int res;
	PrivacyManagerClient* pInst = PrivacyManagerClient::getInstance();

	std::cout << "-- precondition - removeAppPackagePrivacyInfo --" << std::endl;
	res = pInst->removeAppPackagePrivacyInfo("appid2");
	if (res != 0)
	{
		std::cout << "-- precondition - removeAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		//return -1;
	}

	std::cout << "-- addAppPackagePrivacyInfo --" << std::endl;
	std::list < std::string> list3;
	list3.push_back (std::string("http://tizen.org/privilege/contextmanager.privacy11"));
	list3.push_back (std::string("http://tizen.org/privilege/contextmanager.privacydd"));
	list3.push_back (std::string("http://tizen.org/privilege/messaging.writ1e"));
	res = pInst->addAppPackagePrivacyInfo("appid2", list3 );
	if (res != 0)
	{
		std::cout << "-- addAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}
	return 0;
}

int tc_capi(void)
{
	//int i = 111;
	int res;

	//const char *abc[3] = {"abc","def", NULL};

	char** abc = (char**)calloc(10,sizeof (char*));
	abc[0] = (char*) calloc(128,sizeof(char));
	abc[1] = (char*) calloc(128,sizeof(char));
	abc[2] = (char*) calloc(128,sizeof(char));
	abc[3] = (char*) calloc(128,sizeof(char));
	abc[4] = (char*) calloc(128,sizeof(char));
	abc[5] = (char*) calloc(128,sizeof(char));;

	std::cout<<"111"<<std::endl;
	memcpy(abc[0], "http://tizen.org/privilege/bluetooth.admin", 128);
	memcpy(abc[1], "http://tizen.org/privilege/bluetooth.gap", 128);
	memcpy(abc[2], "http://tizen.org/privilege/bluetoothmanager", 128);
	memcpy(abc[3], "http://tizen.org/privilege/contact.read", 128);
	memcpy(abc[4], "http://tizen.org/privilege/messaging.readr", 128);

	res = privacy_manager_client_uninstall_privacy("appid1");

	res = privacy_manager_client_install_privacy("appid1", (const char**)abc);
	std::cout<< " privacy_manager_client_install_privacy result : "<<res  <<std::endl;
	int i = 111;
	res = privacy_manager_client_foreach_privacy_packages(__privacy_manager_client_privacy_packages_cb , &i);
	std::cout<< " privacy_manager_client_get_privacy_packages result : "<<res  <<std::endl;

	res = privacy_manager_client_set_package_privacy("appid1", "http://tizen.org/privacy/bluetooth", false);
	std::cout<< " privacy_manager_client_set_package_privacy result : "<<res  <<std::endl;

	res = privacy_manager_client_foreach_get_privacy_info("appid1", __privacy_manager_client_privacy_info_cb, &i);
	std::cout << "result "  << " : " << res <<std::endl;

	
	res = privacy_manager_client_set_package_privacy("appid1", "http://tizen.org/privacy/bluetooth", true);
	std::cout<< " privacy_manager_client_set_package_privacy result : "<<res  <<std::endl;

	res = privacy_manager_client_foreach_get_privacy_info("appid1", __privacy_manager_client_privacy_info_cb, &i);
	std::cout << "result "  << " : " << res <<std::endl;

	return res;
}
#include <unistd.h>
int tc_check(void)
{
	int res;
	PrivacyManagerClient* pInst = PrivacyManagerClient::getInstance();

	std::cout << "-- precondition - removeAppPackagePrivacyInfo --" << std::endl;
	res = pInst->removeAppPackagePrivacyInfo("appid2");
	if (res != 0)
	{
		std::cout << "-- precondition - removeAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		//return -1;
	}

	std::cout << "-- addAppPackagePrivacyInfo --" << std::endl;
	std::list < std::string> list3;
	list3.push_back (std::string("http://tizen.org/privilege/bluetooth.admin"));
	list3.push_back (std::string("http://tizen.org/privilege/contact.read"));
	list3.push_back (std::string("http://tizen.org/privilege/messaging.read"));
	res = pInst->addAppPackagePrivacyInfo("appid2", list3 );
	if (res != 0)
	{
		std::cout << "-- addAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}

	res = PrivacyChecker::checkWithPrivilege ("appid2", "http://tizen.org/privilege/bluetooth.admin");
	std::cout << "result : " << res << std::endl;

	sleep(1);
	pInst->setPrivacySetting("appid2", "http://tizen.org/privacy/bluetooth", true );
	sleep(1);

	res = PrivacyChecker::checkWithPrivilege ("appid2", "http://tizen.org/privilege/bluetooth.admin");
	std::cout << "result : " << res << std::endl;

	std::cout << "-- postcondition - removeAppPackagePrivacyInfo --" << std::endl;
	res = pInst->removeAppPackagePrivacyInfo("appid2");
	if (res != 0)
	{
		std::cout << "-- postcondition - removeAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		//return -1;
	}
	return 0;
}

int main (int argc, char* argv[])
{


	int res;

//	res = check_addAppPackagePrivacyInfo();

//	std::cout << res << std::endl;

	//res = tc_capi();
	//std::cout << res << std::endl;	

	res = tc_check();
	std::cout << res <<std::endl;
//	int i = 111;
/*
	const char *abc[3] = {"abc","def",NULL};


	abc[0] = (char*) calloc(100,sizeof(char));
	memcpy (abc[1], "priv1", 4);
	abc[1] = (char*) calloc(100,sizeof(char));
	memcpy (abc[1], "priv2", 4);
	abc[2] = (char*) calloc(100,sizeof(char));
	memcpy (abc[1], "priv3", 4);
	abc[3] = NULL;

	res = privacy_manager_client_install_privacy("dddd", abc);
	std::cout<< " privacy_manager_client_install_privacy result : "<<res  <<std::endl;

	res = privacy_manager_client_foreach_privacy_packages(__privacy_manager_client_privacy_packages_cb , &i);
	std::cout<< " privacy_manager_client_get_privacy_packages result : "<<res  <<std::endl;

	res = privacy_manager_client_set_package_privacy("appid1", "privacy1", false);
	std::cout<< " privacy_manager_client_set_package_privacy result : "<<res  <<std::endl;

	res = privacy_manager_client_foreach_get_privacy_info("appid1", __privacy_manager_client_privacy_info_cb, &i);
	std::cout << "result "  << " : " << res <<std::endl;

	
	res = privacy_manager_client_set_package_privacy("appid1", "privacy11", true);
	std::cout<< " privacy_manager_client_set_package_privacy result : "<<res  <<std::endl;

	res = privacy_manager_client_foreach_get_privacy_info("appid1", __privacy_manager_client_privacy_info_cb, &i);
	std::cout << "result "  << " : " << res <<std::endl;
	int res;

	PrivacyChecker::initialize("appid1");
	PrivacyManager* pInst = PrivacyManager::getInstance();

	std::cout << "-- getPrivacyAppPackages --" << std::endl;
	std::list < std::string > list;
	res = pInst->getPrivacyAppPackages(list);
	if (res != 0)
	{
		std::cout << "-- getPrivacyAppPackages: FAILED : " << res << " --" << std::endl;
		return -1;
	}
	std::list < std::string > ::iterator iter = list.begin();
	std::cout <<"list" <<"\n";
	for (; iter != list.end(); ++iter)
	{
		std::cout <<  (*iter) <<"\n";
	}
	
	std::cout << "-- setPrivacySetting --" << std::endl;
	res = pInst->setPrivacySetting("appid1", "privacy1", true);
	if (res != 0)
	{
		std::cout << "-- setPrivacySetting: FAILED : " << res << " --" << std::endl;
		return -1;
	}
	
	std::cout << "-- getAppPackagePrivacyInfo --" << std::endl;
	std::list < std::pair <std::string, bool > >  list2;
	res = pInst->getAppPackagePrivacyInfo("appid1",list2 );
	if (res != 0)
	{
		std::cout << "-- getAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}
	
	for (std::list < std::pair <std::string, bool > >::const_iterator iter = list2.begin(); iter != list2.end(); ++iter)
	{
		std::cout << iter->first << " : " << iter->second << std::endl;
	}

	std::cout << "-- removeAppPackagePrivacyInfo --" << std::endl;
	res = pInst->removeAppPackagePrivacyInfo("appid2");
	if (res != 0)
	{
		std::cout << "-- removeAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		//return -1;
	}
		
	std::cout << "-- addAppPackagePrivacyInfo --" << std::endl;
	std::list < std::string> list3;
	list3.push_back (std::string("privacy1"));
	list3.push_back (std::string("privacy4"));
	list3.push_back (std::string("privacy5"));
	res = pInst->addAppPackagePrivacyInfo("appid2", list3 );
	if (res != 0)
	{
		std::cout << "-- addAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}
	
	std::cout << "-- removeAppPackagePrivacyInfo --" << std::endl;
	pInst->removeAppPackagePrivacyInfo("appid2");
	if (res != 0)
	{
		std::cout << "-- removeAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}

	
	std::cout <<"result of <appid1, privacy1>: " << PrivacyChecker::check("privacy1") << std::endl;
	std::cout <<"result of <appid1, privacy2>: " << PrivacyChecker::check("privacy2") << std::endl;
	std::cout <<"result of <appid1, privacy10>: " << PrivacyChecker::check("privacy10") << std::endl;
	
	
	
	std::cout << "-- setUserPrompted -- " << false  <<std::endl;
	res = pInst->setUserPrompted("appid1", false);
	if (res != 0)
	{
		std::cout << "-- setUserPrompted: FAILED : " << res << " --" << std::endl;
//		return -1;
	}
	bool isPrompted;
	res = pInst->isUserPrompted("appid1", isPrompted);
	if (res != 0)
	{
		std::cout << "-- isUserPrompted: FAILED : " << res << " --" << std::endl;
		return -1;
	}
	std::cout <<"prompted? appid1 : " << isPrompted << std::endl;

	std::cout << "-- setUserPrompted -- " << !isPrompted  <<std::endl;
	res = pInst->setUserPrompted("appid1", !isPrompted);
	if (res != 0)
	{
		std::cout << "-- setUserPrompted: FAILED : " << res << " --" << std::endl;
//		return -1;
	}

	res = pInst->isUserPrompted("appid1", isPrompted);
	std::cout <<"prompted? appid1 : " << isPrompted << std::endl;
		if (res != 0)
	{
		std::cout << "-- setUserPrompted: FAILED : " << res << " --" << std::endl;
//		return -1;
	}

	std::cout << "-- addAppPackagePrivacyInfo --" << std::endl;
	std::list < std::string> list5;
	list5.push_back (std::string("privacy1"));
	list5.push_back (std::string("privacy4"));
	list5.push_back (std::string("privacy5"));
	res = pInst->addAppPackagePrivacyInfo("appid11", list5 );
	if (res != 0)
	{
		std::cout << "-- addAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}
		std::cout << "-- removeAppPackagePrivacyInfo --" << std::endl;
	pInst->removeAppPackagePrivacyInfo("appid11");
	if (res != 0)
	{
		std::cout << "-- removeAppPackagePrivacyInfo: FAILED : " << res << " --" << std::endl;
		return -1;
	}*/
	return 0;
}