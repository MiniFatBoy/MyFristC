#pragma once
#include <Winsock2.h>
#include "mysql.h"
#include <string>
#include <string>
#include <vector>
#include <map>
using namespace std;

class mysql_db
{
public:
	mysql_db();
	~mysql_db();
public:
	/*
	mysql_open()
	return : 1 OK
	-1 失败
	*/
	int mysql_open(const char * host, const char * user, const char * password, const char * database, unsigned int port);
	/*
	mysql_noResult_query();
	非select语句查询
	return >0 成功, 为受影响的行数
	-1 失败
	*/
	int mysql_noResult_query(const char * sql);
	/*
	mysql_select_query();
	有结果集的查询
	return >0 ok 返回结果集条数
	-1 失败
	map_results first = 行 second = values
	*/
	int mysql_select_query(const char * sql, map<int, vector<string>> & map_results);
	/*
	mysql_select_SingleLine_query();
	只有一条数据 , 或者只有一个字段 N 条的查询. 直接调用vector即可
	*/
	int mysql_select_SingleLine_query(const char * sql, vector<string> & v_results);
	/*
	mysql_lasterror();
	返回最近一次错误信息
	*/
	string mysql_lasterror();
private:
	MYSQL sqlCon;
	MYSQL_RES *m_pResult;
	MYSQL_ROW  m_Row;
};
