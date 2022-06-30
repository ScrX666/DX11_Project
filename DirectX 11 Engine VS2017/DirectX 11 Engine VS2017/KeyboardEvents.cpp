#include "KeyboardEvents.h"

KeyboardEvents::KeyboardEvents()
	:
	type(EventType::Invalid),
	key(0u)
{

}

KeyboardEvents::KeyboardEvents(const EventType type, const unsigned char key)
	:
	type(type),
	key(key)
{
	
}
bool KeyboardEvents::isPress() const
{
	return type == EventType::Press;
}
bool KeyboardEvents::isRelease() const
{
	return type == EventType::Release;
}
bool KeyboardEvents::isValid() const
{
	return type == EventType::Invalid;
}
unsigned char KeyboardEvents::GetKeyCode() const
{
	return key;
}
