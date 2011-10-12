#include "ObjectTimer.h"

namespace JungleTime{

ObjectTimer::ObjectTimer(string innerName, string labelName, double cooldown)
	:boost::timer(), innerName(innerName), labelName(labelName), cooldown(cooldown)
{
}


ObjectTimer::~ObjectTimer(void)
{
}

}