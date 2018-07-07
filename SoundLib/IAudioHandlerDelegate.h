#ifndef IAUDIO_HANDLER_DELEGATE_H
#define IAUDIO_HANDLER_DELEGATE_H


namespace SoundLib {
class IAudioHandlerDelegate
{
public:
	virtual ~IAudioHandlerDelegate() = 0;
	virtual void OnPlayedToEnd(const TCHAR* name) = 0;
};
}
#endif
