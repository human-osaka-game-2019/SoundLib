#ifndef ISOUNDS_MANAGER_DELEGATE_H
#define ISOUNDS_MANAGER_DELEGATE_H

#include "IAudioHandlerDelegate.h"


namespace SoundLib {
class ISoundsManagerDelegate : public IAudioHandlerDelegate {
public:
	virtual ~ISoundsManagerDelegate() {}
	virtual void OnPlayedToEnd(TString key) = 0;
};
}
#endif
