#pragma once
#include "SDK.h"

class Movement 
{
public:
	void Bhop();
	void MemeWalk();
}; extern Movement* f_MoveHacks;

class Misc
{
public:
	void ShowRanks();
	void SetClantag(const char* clantag);
}; extern Misc* f_Misc;