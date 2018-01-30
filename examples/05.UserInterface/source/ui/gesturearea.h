#ifndef GESTUREAREA_H
#define GESTUREAREA_H

#include <irrlicht.h>
#include <vector>

#define GESTURE_HoldTime irr::u32(1000)

class GestureArea;

struct GestureEvent;

class GestureListener : irr::IReferenceCounted
{
public:
	GestureListener();

	virtual void onEvent(const GestureEvent &event) = 0;
protected:
	void setGextureArea(GestureArea *area) {}
};

class GestureArea : irr::gui::IGUIElement
{
	struct Touch
	{
		Touch()
		    : id((size_t)-1)
		    , pressed(false)
		    , time_down(0)
		    , time_up(0)
		{}

//		Touch()

		bool operator >(const Touch &o) const {
			return this->id > o.id;
		}

		bool operator ==(const Touch &o) const {
			return this->id == o.id;
		}

		bool  operator ==(const size_t &id) const {
			return this->id == id;
		}

		bool operator =(const Touch &o) {
			id = o.id;
			pressed = o.pressed;
			pos_down = o.pos_down;
			pos_up = o.pos_up;
			time_down = o.time_down;
			time_up = o.time_up;
		}

		size_t                id       ; /**< */
		bool                  pressed  ; /**< */
		irr::core::vector2di  pos_down ; /**< */
		irr::core::vector2di  pos_up   ; /**< */
		irr::u32              time_down; /**< */
		irr::u32              time_up  ; /**< */

		typedef std::vector<Touch>::iterator Iterator;
	};

public:
	GestureArea(irr::gui::EGUI_ELEMENT_TYPE type, irr::IrrlichtDevice* device, IGUIElement* parent,
	            irr::s32 id, const irr::core::rect<irr::s32>& rectangle );

	//! Called if an event happened.
	virtual bool OnEvent(const irr::SEvent& event) _IRR_OVERRIDE_;

	void update();

	void addListener(GestureListener* listener);

protected:
	irr::u32                      m_touchCount;
	std::vector<Touch>            m_touch;
	std::vector<GestureListener*> m_listener;
	irr::IrrlichtDevice          *m_device;
};

enum GestureType {
	EGT_HOLD,
	EGT_TAP,
	EGT_PINCH,
	EGT_ZOOM,
	EGT_ROTATE,
	EGT_DRAG
};

struct GestureEvent {

	struct THoldGesture {
		irr::u32             HoldTime;
		irr::core::vector2di Position;
	};

	GestureType Type;

	union {
		THoldGesture HoldGesture;
	};
};

#endif // GESTUREAREA_H
