// Ŭnicode please 
#include "stdafx.h"
#include "console_function.h"
#include "OgreConsoleForGorilla.h"

namespace console {;
void whoami(Ogre::StringVector&)
{
	OgreConsole::getSingleton().print("You're you!");
}

void version(Ogre::StringVector&)
{
	std::stringstream s;
	s << "Ogre " << OGRE_VERSION_MAJOR << "." << OGRE_VERSION_MINOR << "." << OGRE_VERSION_PATCH << " '" << OGRE_VERSION_NAME << "'";
	OgreConsole::getSingleton().print(s.str());
}

void make(Ogre::StringVector& vec)
{
	if (vec.size() == 1)
		OgreConsole::getSingleton().print("Make you what?");
	else
	{
		Ogre::String item_to_make = vec[1];
		Ogre::StringUtil::toLowerCase(item_to_make);
		if (item_to_make == "sandwich")
			OgreConsole::getSingleton().print("Make it yourself!");
		else if (item_to_make == "universe")
			OgreConsole::getSingleton().print("Boom!");
		else if (item_to_make == "ogre")
			OgreConsole::getSingleton().print("I need a Daddy and Mommy Ogre");
		else if (item_to_make == "gorilla")
			OgreConsole::getSingleton().print("He wouldn't like that");
		else
			OgreConsole::getSingleton().print("Go check your fridge.");
	}
}

void sudo(Ogre::StringVector& vec)
{
	// See http://xkcd.com/149/ for the design specs of this function.
	if( vec.size() == 3 )
	{
		Ogre::String make = vec[1];
		Ogre::String item_to_make = vec[2];
		Ogre::StringUtil::toLowerCase(make);
		Ogre::StringUtil::toLowerCase(item_to_make);
		if( make == "make" && item_to_make == "sandwich")
		{
			OgreConsole::getSingleton().print("Oh, ok then . . . . . . argh! The Ogre ate it! Honest!");
			return;
		}
	}
	OgreConsole::getSingleton().print("Go check everyones fridge.");
}

}	// namespace console