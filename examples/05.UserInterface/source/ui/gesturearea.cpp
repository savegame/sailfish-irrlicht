#include "gesturearea.h"
#include <algorithm>

using namespace irr;
using namespace gui;
GestureArea::GestureArea(EGUI_ELEMENT_TYPE type, IGUIEnvironment *environment, IGUIElement *parent,
                         s32 id, const core::rect<irr::s32>& rectangle)
    :irr::gui::IGUIElement(type,environment, parent, id, rectangle)
{

}

bool GestureArea::OnEvent(const SEvent &event)
{
	if( event.EventType != EET_TOUCH_INPUT_EVENT )
		return Parent ? Parent->OnEvent(event) : false;
	size_t id = (size_t)-1;
	std::vector<Touch>::iterator it;
	SEvent::STouchInput current_touch = event.TouchInput;
	switch( current_touch.Event )
	{
	case ETIE_MOVED:
		break;
	case ETIE_PRESSED_DOWN:
		it = std::find(m_touch.begin(),m_touch.end(), id);
		if( it != m_touch.end() )
		{
			(*it).id               = current_touch.ID;
			(*it).position.X       = current_touch.X;
			(*it).position.Y       = current_touch.Y;
			(*it).pressed = true;
		}
		break;
	case ETIE_LEFT_UP:
		it = std::find(m_touch.begin(),m_touch.end(), current_touch.ID);
		if( it != m_touch.end() )
		{
			(*it).id               = id;
			(*it).position.X       = current_touch.X;
			(*it).position.Y       = current_touch.Y;
			(*it).pressed = false;
		}
		break;
	}
	return false;
}
