#ifndef GESTUREAREA_H
#define GESTUREAREA_H

#include <irrlicht.h>
#include <vector>

class GestureArea : irr::gui::IGUIElement
{
	struct Touch
	{
		Touch() : id((size_t)-1), pressed(false)
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

		size_t                id      ; /**< */
		bool                  pressed ; /**< */
		irr::core::vector2di  position; /**< */
	};

public:
	GestureArea(irr::gui::EGUI_ELEMENT_TYPE type, irr::gui::IGUIEnvironment* environment, IGUIElement* parent,
	            irr::s32 id, const irr::core::rect<irr::s32>& rectangle );

	//! Called if an event happened.
	virtual bool OnEvent(const irr::SEvent& event) _IRR_OVERRIDE_;
protected:
	std::vector<Touch> m_touch;
};

#endif // GESTUREAREA_H
