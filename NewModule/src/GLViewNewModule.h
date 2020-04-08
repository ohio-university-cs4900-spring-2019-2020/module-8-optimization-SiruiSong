#pragma once

#include "GLView.h"
#include "SoundManager.h"
#include "irrKlang.h"
#include "FrustumCullingGen.h"
#include "WOFTGLString.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewNewModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewNewModule : public GLView
{
public:
   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNewModuleWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

protected:
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate();   

   SoundManager* smgr;
   std::vector<WO*> carWOs;
   std::vector<WOFTGLString*> labelWOs;
   FrustumCullingGen* fc;
   WO* frustum;
};

/** \} */

} //namespace Aftr
