#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"

//If we want to use way points, we need to include this.
#include "NewModuleWayPoints.h"
#include "MGLFTGLString.h"
#include "AftrGeometryFrustum.h"
#include "MGLFrustum.h"

#include "io.h"
#include <iostream>
#include <string>

using namespace Aftr;
using namespace std;

GLViewNewModule* GLViewNewModule::New( const std::vector< std::string >& args )
{
   GLViewNewModule* glv = new GLViewNewModule( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewNewModule::GLViewNewModule( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewNewModule::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1

   // Check if the Debug folder in the right place.
   if ((_access("../irrKlang-64bit-1.6.0/music/ophelia.mp3", 0)) == -1) {
	   cout << "Sound file path error.." << endl;
	   cout << "Should put the 'Debug' folder under 'NewModule' directly." << endl;
	   return;
   }

   //Set background music with 2D Sound when starting the program.
   this->smgr = SoundManager::init();
   this->smgr->play2D("../irrKlang-64bit-1.6.0/music/ophelia.mp3", true, false, true);
   this->smgr->getSound2D().at(0)->setVolume(0.5f);
    
   this->fc = new FrustumCullingGen();
}


GLViewNewModule::~GLViewNewModule()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.

   for (int i = 0; i < carWOs.size(); i++) {
       if (this->fc->isCameraVisible(carWOs[i])) {
           this->carWOs[i]->isVisible = true;
       }
       else {
           this->carWOs[i]->isVisible = false;
       }

       if (this->fc->isFrustumVisible(carWOs[i], this->frustum)) {
           this->labelWOs[i]->getModelT<MGLFTGLString>()->setText("IN FRUSTUM");
           this->labelWOs[i]->getModelT<MGLFTGLString>()->setFontColor(aftrColor4f(1.0f, 0.5f, 1.5f, 1.0f));
           this->labelWOs[i]->getModelT<MGLFTGLString>()->setSize(10, 10);
       }
       else {
           this->labelWOs[i]->getModelT<MGLFTGLString>()->setText("NOT IN FRUSTUM");
           this->labelWOs[i]->getModelT<MGLFTGLString>()->setFontColor(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f));
           this->labelWOs[i]->getModelT<MGLFTGLString>()->setSize(5, 5);
       }
   }
}


void GLViewNewModule::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewNewModule::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewNewModule::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );

}


void GLViewNewModule::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );

}


void GLViewNewModule::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if (key.keysym.sym == SDLK_r)
   {
       /// rotate frustum
       Mat4 frustumMatrix = this->frustum->getDisplayMatrix();
       frustumMatrix = frustumMatrix.rotate(Vector(0, 0, 1), PI / 10.0f);
       this->frustum->getModel()->setDisplayMatrix(frustumMatrix);
   }
}


void GLViewNewModule::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );

}


void Aftr::GLViewNewModule::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,20 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 0, 0, 100 ) );
   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   ////Create the infinite grass plane (the floor)
   wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector( 0, 0, 0 ) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );

   //wo = WO::New(wheeledCar, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
   //wo->setPosition(15, 50, 0);
   //wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //worldLst->push_back(wo);
   float nearPlane = 1.0f; // Distance from "eyeball" to the frustum's near plane
   float farPlane = 30.0f; // Distance from "eyeball" to the frustum's far plane
   float HFOV = 65.0f; // Horizontal Field-of-view in degrees
   float aspectRatio = 16.0f / 9.0f; // Vertical Field-of-view in degrees

   //car1->getModel()->getNearestPointWhereLineIntersectsMe(rayTail, rayHead, outputPoint);
   for (int i = 0; i < 10; i++) {
       WO* car = WO::New(wheeledCar, Vector(1.2f, 1.2f, 1.2f), MESH_SHADING_TYPE::mstFLAT);
       //car->setModel(MGLFrustum::New(car, nearPlane, farPlane, HFOV, aspectRatio));
       car->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
       car->getModel()->renderBBox = true;
       car->setPosition(15 + sin(i * 30) * 30, 45 - cos(i * 30) * 30, 10);
       worldLst->push_back(car);
       this->carWOs.push_back(car);

       WOFTGLString* string = WOFTGLString::New(ManagerEnvironmentConfiguration::getSMM() + "/fonts/COMIC.TTF", 30);
       string->getModelT<MGLFTGLString>()->setFontColor(aftrColor4f(1.0f, 0.5f, 1.5f, 1.0f));
       string->getModelT<MGLFTGLString>()->setSize(10, 10);
       string->getModelT<MGLFTGLString>()->setText("IN FRUSTUM");
       string->rotateAboutGlobalX(PI / 2);
       string->rotateAboutGlobalZ(-PI / 2);
       string->setPosition(car->getPosition().x, car->getPosition().y, car->getPosition().z + 5);
       worldLst->push_back(string);
       this->labelWOs.push_back(string);
   }

   this->frustum = WO::New();
   this->frustum->setModel(MGLFrustum::New(this->frustum, nearPlane, farPlane, HFOV, aspectRatio));
   this->frustum->setPosition(15, 45, 10);
   this->frustum->setLabel("Frustum");
   worldLst->push_back(this->frustum);

   //AftrGeometryFrustum* frustum = new AftrGeometryFrustum(aspectRatio, VFOV, nearPlane, farPlane, this->cam->getLookDirection(),
   //    this->cam->getNormalDirection(), this->cam->getPosition());
   //frustum->getPlaneNormal(5);
   //frustum->getPlaneCoef(5);

   createNewModuleWayPoints();
}


void GLViewNewModule::createNewModuleWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
