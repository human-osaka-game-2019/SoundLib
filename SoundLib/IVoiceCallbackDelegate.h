#ifndef IVOICE_CALLBACK_DELEGATE_H
#define IVOICE_CALLBACK_DELEGATE_H


namespace SoundLib {
class IVoiceCallbackDelegate {
public:
	virtual ~IVoiceCallbackDelegate() = default;
	virtual void BufferEndCallback() = 0;
};
}
#endif
