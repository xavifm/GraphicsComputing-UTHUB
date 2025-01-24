#pragma once

#include "../Parameters/Globals.h"

class Application;

class Controller
{
public:

	Controller()
	{
	}

	virtual ~Controller()
	{
	}

	virtual bool Init()
	{
		return true;
	}

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}
};