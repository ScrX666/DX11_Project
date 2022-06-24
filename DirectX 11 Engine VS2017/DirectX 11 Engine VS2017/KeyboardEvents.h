#pragma once
class KeyboardEvents
{
public:
	enum EventType
	{
		Press,Release,Invalid
	};
	KeyboardEvents();
	KeyboardEvents(const EventType type, const unsigned char key);
	bool isPress() const;
	bool isRelease() const;
	bool isValid() const;
	unsigned char GetKeyCode() const;
private:
	EventType type;
	unsigned char key;
};
