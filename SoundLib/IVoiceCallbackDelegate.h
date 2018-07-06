#ifndef IVOICE_CALLBACK_DELEGATE_H
#define IVOICE_CALLBACK_DELEGATE_H


namespace SoundLib {
class IVoiceCallbackDelegate {
public:
	virtual ~IVoiceCallbackDelegate() {}
	virtual void BufferEndCallback(void) = 0;
};
}
#endif
