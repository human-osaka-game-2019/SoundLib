#ifndef ISOUNDS_MANAGER_DELEGATE_H
#define ISOUNDS_MANAGER_DELEGATE_H

#include "IAudioHandlerDelegate.h"


namespace SoundLib {
class ISoundsManagerDelegate : public IAudioHandlerDelegate {
public:
	virtual ~ISoundsManagerDelegate() = 0;
	virtual void OnPlayedToEnd(const TCHAR* pKey) = 0;
};
}
#endif
