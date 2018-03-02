#include "gameapp.h"
#include "../ui/gesturearea.h"
#include <irrlicht.h>

GameApp::GameApp(int argc, char **argv) {
    m_device = nullptr;
    
    irr::SIrrlichtCreationParameters parameters;
    parameters.DriverType = IRR_DRIVER_TYPE;
    parameters.Bits = 32;
    parameters.AntiAlias = false;
    parameters.WindowSize = irr::core::dimension2du(480,800);
    
    m_device = irr::createDeviceEx(parameters);
    
    if(m_device == nullptr)
        return;
    
	m_driver = m_device->getVideoDriver();
	m_gui = m_device->getGUIEnvironment();
}

GameApp::~GameApp() {
    if( nullptr == m_device )
        return;
    m_device->drop();
}

irr::IrrlichtDevice *GameApp::device() const {
    return m_device;
}

irr::video::IVideoDriver *GameApp::driver() const {
    return m_driver;
}

int GameApp::run() { 
    if(nullptr == m_device)
        return 1;
    
    while( m_device->run() )
    if( m_device->isWindowActive() )
    {
        m_driver->beginScene((irr::u16)0, irr::video::SColor(255,0,0,0), (irr::f32)1.f, (irr::u8)0);
        m_device->getSceneManager()->drawAll();
        m_driver->endScene();
    }
    return 0;
}




