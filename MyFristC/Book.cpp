#include "stdafx.h"
#include "Book.h"
#include <stdarg.h>

using namespace std;

book::book()
{
	
}

book::~book()
{
	
}

void book::setPrice(double price)
{
this->price = price;
}

double book::getPrice() const
{
	return  price;
}

void book::setTitle(char * t_char)
{
	title = t_char;
}

char * book::getTitle() const
{
	return title;
}
