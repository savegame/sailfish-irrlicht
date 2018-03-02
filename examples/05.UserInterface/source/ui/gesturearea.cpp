#include "gesturearea.h"
#include <algorithm>

using namespace irr;
using namespace gui;


GestureListener::GestureListener() : irr::IReferenceCounted()
{

}


GestureArea::GestureArea(EGUI_ELEMENT_TYPE type, IrrlichtDevice *device, IGUIElement *parent,
                         s32 id, const core::rect<irr::s32>& rectangle)
    :irr::gui::IGUIElement(type,device->getGUIEnvironment(), parent, id, rectangle)
    , m_device(device)
    , m_touchCount(0)
{
//	m_device = device;
}

bool GestureArea::OnEvent(const SEvent &event)
{
	if( event.EventType != EET_TOUCH_INPUT_EVENT )
		return Parent ? Parent->OnEvent(event) : false;
	size_t id = (size_t)-1;
	Touch::Iterator it;
	SEvent::STouchInput current_touch = event.TouchInput;
	switch( current_touch.Event )
	{
	case ETIE_MOVED:
		it = std::find(m_touch.begin(),m_touch.end(),  current_touch.ID);
		if( it != m_touch.end() )
		{
			(*it).pos_up.X   = current_touch.X;
			(*it).pos_up.Y   = current_touch.Y;
			//(*it).time       = current_touch.Time;
			(*it).pressed    = true;
		}
		break;
	case ETIE_PRESSED_DOWN:
		it = std::find(m_touch.begin(),m_touch.end(), id);
		if( it != m_touch.end() )
		{
			(*it).id         = current_touch.ID;
			(*it).pos_down.X = current_touch.X;
			(*it).pos_down.Y = current_touch.Y;
			(*it).pos_up.X   = current_touch.X;
			(*it).pos_up.Y   = current_touch.Y;
			(*it).time_down  = m_device->getTimer()->getTime();
//			this->->getT
			(*it).pressed    = true;
			m_touchCount++;
		}
		break;
	case ETIE_LEFT_UP:
		it = std::find(m_touch.begin(),m_touch.end(), current_touch.ID);
		if( it != m_touch.end() )
		{
			(*it).id         = id;
			(*it).pos_up.X   = current_touch.X;
			(*it).pos_up.Y   = current_touch.Y;
			(*it).time_up    = m_device->getTimer()->getTime();
			(*it).pressed    = false;
			m_touchCount--;
		}
		break;
	}
	return false;
}

void GestureArea::update()
{
	Touch::Iterator
	        it = m_touch.begin(),
	        end = m_touch.end();
	for( ; it != end; it++ )
	{
		Touch &touch = (*it);
		if( touch.id != -1 )
		{
			irr::u32 deltaTime = m_device->getTimer()->getTime() - touch.time_down;
			if( m_touchCount == 1
			        && touch.pressed
			        && deltaTime >= GESTURE_HoldTime )
			{// hold gesture if it in specific rect
				core::vector2di vec = touch.pos_up - touch.pos_down;
				if( vec.getLengthSQ() <= 100 )
				{
					GestureEvent event;
					event.Type = GestureType::EGT_HOLD;
					event.HoldGesture.HoldTime = m_device->getTimer()->getTime() - touch.time_down;
					event.HoldGesture.Position = touch.pos_down;
					for(int i = 0; i < m_listener.size(); i++ )
						m_listener[i].onEvent(event);
					break;
				}
			}
		}
	}
}

void GestureArea::addListener(GestureListener *listener)
{
	listener->grab();
	m_listener.push_back(listener);
}

