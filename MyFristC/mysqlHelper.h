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
	-1 ʧ��
	*/
	int mysql_open(const char * host, const char * user, const char * password, const char * database, unsigned int port);
	/*
	mysql_noResult_query();
	��select����ѯ
	return >0 �ɹ�, Ϊ��Ӱ�������
	-1 ʧ��
	*/
	int mysql_noResult_query(const char * sql);
	/*
	mysql_select_query();
	�н�����Ĳ�ѯ
	return >0 ok ���ؽ��������
	-1 ʧ��
	map_results first = �� second = values
	*/
	int mysql_select_query(const char * sql, map<int, vector<string>> & map_results);
	/*
	mysql_select_SingleLine_query();
	ֻ��һ������ , ����ֻ��һ���ֶ� N ���Ĳ�ѯ. ֱ�ӵ���vector����
	*/
	int mysql_select_SingleLine_query(const char * sql, vector<string> & v_results);
	/*
	mysql_lasterror();
	�������һ�δ�����Ϣ
	*/
	string mysql_lasterror();
private:
	MYSQL sqlCon;
	MYSQL_RES *m_pResult;
	MYSQL_ROW  m_Row;
};
