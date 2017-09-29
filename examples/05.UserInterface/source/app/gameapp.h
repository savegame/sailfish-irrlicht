//
//  gameapp.h
//  DarkTime
//
//  Created by Savegame on 26.09.17.
//

#ifndef gameapp_h
#define gameapp_h

#ifdef SAILFIH
#  define IRR_DRIVER_TYPE irr::video::EDT_OGLES2
#else
#  define IRR_DRIVER_TYPE irr::video::EDT_OPENGL
#endif

namespace irr {
    class IrrlichtDevice;
    namespace video {
        class IVideoDriver;
    }
}

class GameApp
{
public:
    GameApp(int argc, char **argv);
    
    ~GameApp();
    
    int run();
    irr::IrrlichtDevice *device() const;
    irr::video::IVideoDriver *driver() const;
protected:
    irr::IrrlichtDevice *m_device;
    irr::video::IVideoDriver *m_driver;
};
#endif /* gameapp_h */
