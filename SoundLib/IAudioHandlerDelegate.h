#ifndef IAUDIO_HANDLER_DELEGATE_H
#define IAUDIO_HANDLER_DELEGATE_H


namespace SoundLib {
class IAudioHandlerDelegate
{
public:
	virtual ~IAudioHandlerDelegate() = default;
	virtual void OnPlayedToEnd(TString name) = 0;
};
}
#endif
