#ifndef ISOUNDS_MANAGER_DELEGATE_H
#define ISOUNDS_MANAGER_DELEGATE_H

#include "IAudioHandlerDelegate.h"

class ISoundsManagerDelegate : public IAudioHandlerDelegate {
public :
	virtual ~ISoundsManagerDelegate() = 0;
	virtual void OnPlayedToEnd(const char* pKey) = 0;
};

#endif
