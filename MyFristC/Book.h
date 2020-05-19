#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <iostream>


using namespace std;

class book
{
public:
	book();
	~book();
	void setPrice(double price);
	double getPrice() const;
	void setTitle(char * t_char);
	char * getTitle() const;
private:
	char * title;
	double price;
};
