#pragma once
#include "CustomWindows.h"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};