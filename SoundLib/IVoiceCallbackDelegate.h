#ifndef IVOICE_CALLBACK_DELEGATE_H
#define IVOICE_CALLBACK_DELEGATE_H

class IVoiceCallbackDelegate {
public:
	virtual void BufferEndCallback(void) = 0;
};

#endif
