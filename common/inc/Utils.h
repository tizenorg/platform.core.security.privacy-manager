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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
#include <iostream>
#include <algorithm>
#include <dlog.h>
#include <sqlite3.h>
#include <memory>
#include <string>

#define	TryCatchLogReturn(condition, expr, r, logFormat)	if (!(condition)) { \
		LOGE(logFormat); \
		expr; \
		return r; \
	} else {;}

#define	TryCatchResLogReturn(condition, expr, r, logFormat, res)	if (!(condition)) { \
		LOGE(logFormat, res); \
		expr; \
		return r; \
	} else {;}


#define TryReturn(condition, r, expr, ...) \
	if ( !(condition) ) { \
		LOGE(__VA_ARGS__); \
		expr; \
		return r; \
	} else {;}

auto StmtDeleter = [&](sqlite3_stmt* pPtr) { /*LOGI("sqlite3_finalize");*/ sqlite3_reset (pPtr); sqlite3_finalize(pPtr); };
auto DbDeleter = [&](sqlite3* pPtr) { /*LOGI("sqlite3_close");*/ sqlite3_close(pPtr); };

#define setStmtToUniquePtr(x, y)		std::unique_ptr < sqlite3_stmt, decltype(StmtDeleter) > x (y, StmtDeleter);
#define setDbToUniquePtr(x, y)			std::unique_ptr < sqlite3, decltype(DbDeleter) > x (y, DbDeleter);

#define openDb(dbpath, pHandler, mode)	sqlite3* pHandler##Temp = NULL;\
	{\
		/*LOGI("sqlite3_open_v2");*/\
		int res = sqlite3_open_v2(dbpath, &pHandler##Temp, mode , NULL);\
		TryCatchResLogReturn( res == SQLITE_OK, , PRIV_MGR_ERROR_DB_ERROR, "sqlite3_open : %d", res);\
	}\
	setDbToUniquePtr(pHandler, pHandler##Temp);\
	
#define prepareDb(pHandler, sql, pStmt)	sqlite3_stmt* pStmt##Temp;\
	{\
		/*LOGI("sqlite3_prepare_v2");*/\
		int res = sqlite3_prepare_v2(pHandler.get(), sql, -1, & pStmt##Temp, NULL);\
		TryCatchResLogReturn( res == SQLITE_OK, , PRIV_MGR_ERROR_DB_ERROR, "sqlite3_prepare_v2 : %d", res);\
	}\
	setStmtToUniquePtr(pStmt, pStmt##Temp);

class Utils
{
public:
	static std::string toHash (std::string src);
};
#endif //_UTILS_H_
