#ifndef IAUDIO_HANDLER_DELEGATE_H
#define IAUDIO_HANDLER_DELEGATE_H

class IAudioHandlerDelegate
{
public:
	virtual ~IAudioHandlerDelegate() = 0;
	virtual void OnPlayedToEnd(const char* name) = 0;
};

#endif
