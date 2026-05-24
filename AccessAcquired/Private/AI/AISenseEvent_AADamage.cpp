// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AISenseEvent_AADamage.h"
#include "AISense_AADamage.h"

FAISenseID UAISenseEvent_AADamage::GetSenseID() const
{
	return UAISense::GetSenseID<UAISense_AADamage>();
}
