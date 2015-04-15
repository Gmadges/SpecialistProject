#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Texture.h>
#include <QGLWidget>
#include <QFont>


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene(QWindow *_parent) : OpenGLWindow(_parent)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  setTitle("Simple OpenGL Texture");

  m_speed=0;
  m_repeat=1;

}

void NGLScene::loadTexture()
{
    static unsigned int seed = 23234;
    srand(seed);

    unsigned int width = 1028;
    unsigned int height = 1028;
    unsigned int index = 0;
    unsigned int density = 10000;

    //create pixel array of data set it all to zero
    GLfloat *data = new GLfloat[width*height*4];

    for( unsigned int y=0; y < height; ++y)
    {
        for( unsigned int x = 0; x < width; ++x)
        {
            //red
            data[index++]= 0.0f;
            //green
            data[index++]= 0.0f;
            //blue
            data[index++]= 0.0f;
            //alpha the important one
            data[index++]= 1.0f;
        }
    }

    //randomly add hairs
    for(unsigned int tmp = 0; tmp < density; tmp++)
    {
        int pixel = rand() % (height*width)+1;

        //red
        data[pixel*4]= 0.0f;
        //green
        data[pixel*4+1]= 0.75f;
        //blue
        data[pixel*4+2]= 0.0f;
        //alpha the important one
        data[pixel*4+3]= 1.0f;
    }


    glGenTextures(1,&m_textureName);
    glBindTexture(GL_TEXTURE_2D,m_textureName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,0, GL_RGBA, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_2D); //  Allocate the mipmaps

    delete [] data;

}

NGLScene::~NGLScene()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  Init->NGLQuit();
  // remove the texture now we are done
  glDeleteTextures(1,&m_textureName);

}



void NGLScene::resizeEvent(QResizeEvent *_event )
{
  if(isExposed())
  {
    int w=_event->size().width();
    int h=_event->size().height();
    // set the viewport for openGL
    glViewport(0,0,w,h);
    // now set the camera size values as the screen size has changed
    m_cam->setShape(45,(float)w/h,0.05,350);
    renderLater();
  }
}


void NGLScene::initialize()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::instance();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,5,5);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_cam= new ngl::Camera(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam->setShape(45,(float)720.0/576.0,0.5,160);
  // now to load the shader and set the values
  // grab an instance of shader manager

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  /*phong shader
  shader->createShaderProgram("Phong");

  shader->attachShader("PhongVertex",ngl::VERTEX);
  shader->attachShader("PhongFragment",ngl::FRAGMENT);
  shader->loadShaderSource("PhongVertex","shaders/PhongVertex.glsl");
  shader->loadShaderSource("PhongFragment","shaders/PhongFragment.glsl");

  shader->compileShader("PhongVertex");
  shader->compileShader("PhongFragment");
  shader->attachShaderToProgram("Phong","PhongVertex");
  shader->attachShaderToProgram("Phong","PhongFragment");

  shader->linkProgramObject("Phong");
  */

  //create fur shader stuff
  shader->createShaderProgram("FurShader");

  shader->attachShader("FurVertex",ngl::VERTEX);
  shader->attachShader("FurFragment",ngl::FRAGMENT);

  shader->loadShaderSource("FurVertex","shaders/FurVert.glsl");
  shader->loadShaderSource("FurFragment","shaders/FurFrag.glsl");

  shader->compileShader("FurVertex");
  shader->compileShader("FurFragment");

  shader->attachShaderToProgram("FurShader","FurVertex");
  shader->attachShaderToProgram("FurShader","FurFragment");


  shader->linkProgramObject("FurShader");
  shader->use("FurShader");

  //test geo shaders
  shader->createShaderProgram("normalShader");

  shader->attachShader("normalVertex",ngl::VERTEX);
  shader->attachShader("normalFragment",ngl::FRAGMENT);
  shader->loadShaderSource("normalVertex","shaders/normalVertex.glsl");
  shader->loadShaderSource("normalFragment","shaders/normalFragment.glsl");

  shader->compileShader("normalVertex");
  shader->compileShader("normalFragment");
  shader->attachShaderToProgram("normalShader","normalVertex");
  shader->attachShaderToProgram("normalShader","normalFragment");

  shader->attachShader("normalGeo",ngl::GEOMETRY);
  shader->loadShaderSource("normalGeo","shaders/normalGeo.glsl");
  shader->compileShader("normalGeo");
  shader->attachShaderToProgram("normalShader","normalGeo");


  shader->linkProgramObject("normalShader");
  shader->use("normalShader");


  // now pass the modelView and projection values to the shader
  shader->setShaderParam1i("xMultiplyer",m_repeat);
  shader->setShaderParam1f("yOffset",0.0);

  loadTexture();

  glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
  // timer needs starting after gl context created
  m_texTimer = startTimer(50);

  prim->createSphere("ball", 2.0f, 50.0f);

}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MVP=m_mouseGlobalTX*m_cam->getVPMatrix();

  shader->setShaderParamFromMat4("MVP",MVP);

}

void NGLScene::render()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["FurShader"]->use();
	glBindTexture(GL_TEXTURE_2D,m_textureName);

	shader->setShaderParam1i("xMultiplyer",m_repeat);

	// now we bind back our vertex array object and draw

  loadMatricesToShader();
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->draw("ball");
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    renderLater();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    renderLater();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	renderLater();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;

  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Minus : incrementSpeed(); break;
  case Qt::Key_Plus : decrementSpeed(); break;
  case Qt::Key_Left : incrementRepeat(); break;
  case Qt::Key_Right : decrementRepeat(); break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    renderLater();
}


void NGLScene::timerEvent(QTimerEvent *_event )
{
	// re-draw GL
	if(_event->timerId() == m_texTimer)
	{
		static float offset=0.0;

    offset+=m_speed;
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    shader->use("FurShader");
    shader->setShaderParam1f("yOffset",offset);
  }
  renderNow();
}


