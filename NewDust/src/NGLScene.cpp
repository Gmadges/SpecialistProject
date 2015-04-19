#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>

#include<QGLWidget>


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=5.0;
//----------------------------------------------------------------------------------------------------------------------
/// num instances
//----------------------------------------------------------------------------------------------------------------------
const static unsigned int maxinstances=1000000;



//----------------------------------------------------------------------------------------------------------------------
void NGLScene::incInstances()
{
  m_instances+=10000;
  if(m_instances>maxinstances)
    m_instances=maxinstances;
  m_updateBuffer=true;
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::decInstances()
{
  m_instances-=10000;
  if(m_instances<10000)
    m_instances=10000;
  m_updateBuffer=true;
}

NGLScene::NGLScene(QWindow *_parent) : OpenGLWindow(_parent)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  setTitle("Dust");
  m_fpsTimer =startTimer(0);
  m_fps=0;
  m_frames=0;
  m_timer.start();
  m_polyMode=GL_FILL;
  m_instances=1000;
  m_updateBuffer=true;

}


NGLScene::~NGLScene()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  glDeleteVertexArrays(1,&m_dataID);
  glDeleteVertexArrays(1,&m_matrixID);
  glDeleteVertexArrays(1,&m_vaoID);
  Init->NGLQuit();
}

void NGLScene::loadTexture()
{
  QImage image;

  bool loaded = image.load("textures/Sprite.png", "PNG");
  if(loaded == true)
  {

  QImage GLimage;
  GLimage = QGLWidget::convertToGLFormat(image);

  glGenTextures(1,&m_textureName);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,m_textureName);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               GLimage.width(),
               GLimage.height(),
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               GLimage.bits()
               );
  }
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

void NGLScene::InitDrawingShaders()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    shader->createShaderProgram("TextureShader");

    shader->attachShader("TextureVertex",ngl::VERTEX);
    shader->attachShader("TextureFragment",ngl::FRAGMENT);
    shader->loadShaderSource("TextureVertex","shaders/drawVertex.glsl");
    shader->loadShaderSource("TextureFragment","shaders/drawFragment.glsl");

    shader->compileShader("TextureVertex");
    shader->compileShader("TextureFragment");
    shader->attachShaderToProgram("TextureShader","TextureVertex");
    shader->attachShaderToProgram("TextureShader","TextureFragment");

    // link
    shader->linkProgramObject("TextureShader");
    shader->use("TextureShader");
    // register the uniforms for later uses
    shader->autoRegisterUniforms("TextureShader");
}

void NGLScene::particleSystemInit()
{
    Particle Particles[10000];
    
    //create particle data.

    Particles[0].Type = 1;
    Particles[0].Pos = ngl::Vec3(0,0,0);
    Particles[0].Vel = ngl::Vec3(0.0f, 0.0001f, 0.0f);
    Particles[0].LifetimeMillis = 0.0f;
    
    glGenTransformFeedbacks(2, m_transformFeedback); 
    glGenBuffers(2, m_particleBuffer);
    
    for (unsigned int i = 0; i < 2 ; i++) 
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
    }


    // here we see what the max size of a uniform block can be, this is going
    // to be the GL_MAX_UNIFORM_BLOCK_SIZE / the size of the data we want to pass
    // into the uniform block which in this case is just a series of ngl::Mat4
    // in the case of the mac it's 1024
    GLint maxUniformBlockSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
    m_instancesPerBlock = maxUniformBlockSize / sizeof(ngl::Mat4);
    std::cout<<"Number of instances per block is "<<m_instancesPerBlock<<"\n";

    // now to load the shader and set the values
    // grab an instance of shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    // This is for our transform shader and it will load a series of matrics into a uniform
    // block ready for drawing later
    shader->createShaderProgram("TransformFeedback");
    shader->attachShader("TransVertex",ngl::VERTEX);
    shader->loadShaderSource("TransVertex","shaders/updateFeedback.glsl");
    shader->compileShader("TransVertex");
    shader->attachShaderToProgram("TransformFeedback","TransVertex");
    // bind our attribute
    shader->bindAttribute("TransformFeedback",0,"inPos");
    // now we want to get the TransformFeedback variables and set them
    // first get the shader ID
    GLuint id=shader->getProgramID("TransformFeedback");
    // create a list of the varyings we want to attach to (this is the out in our shader)
    const char *varyings[1] = { "ModelView" };
    // The names of the vertex or geometry shader outputs to be recorded in
    // transform feedback mode are specified using glTransformFeedbackVaryings
    // in this case we are storing 1 (ModelView) and the attribs are
    glTransformFeedbackVaryings(id, 1, varyings, GL_INTERLEAVED_ATTRIBS);
    // now link the shader
    shader->linkProgramObject("TransformFeedback");
    shader->use("TransformFeedback");
    // register the uniforms
    shader->autoRegisterUniforms("TransformFeedback");

    // create our cube
    shader->setShaderParam1i("tex1",0);

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
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,1,220);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);

  m_cam= new ngl::Camera(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam->setShape(45,(float)720.0/576.0,0.5,150);

  loadTexture();

  glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces

  m_text = new ngl::Text(QFont("Arial",14));
  m_text->setScreenSize(width(),height());
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  ngl::Mat4 M;
  M=m_transform.getMatrix()*m_mouseGlobalTX;
  MV=  M*m_cam->getViewMatrix();
  MVP= M*m_cam->getVPMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
}

void NGLScene::updateParticles()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();


    (*shader)["TransformFeedback"]->use();
    // if the number of instances have changed re-bind the buffer to the correct size

    glBindBuffer(GL_ARRAY_BUFFER, m_matrixID);
    glBufferData(GL_ARRAY_BUFFER, m_instances*sizeof(ngl::Mat4), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_matrixID);
    glBindTexture(GL_TEXTURE_BUFFER,m_tboID);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_matrixID);



    //----------------------------------------------------------------------------------------------------------------------
    // SETUP DATA
    //----------------------------------------------------------------------------------------------------------------------
    glBindTexture(GL_TEXTURE_BUFFER, m_tboID);

    // activate our vertex array for the points so we can fill in our matrix buffer
    glBindVertexArray(m_dataID);
    // set the view for the camera
    shader->setRegisteredUniformFromMat4("View",m_cam->getViewMatrix());
    // this sets some per-vertex data values for the Matrix shader
    shader->setRegisteredUniform4f("data",0.3,0.6,0.5,1.2);
    // pass in the mouse rotation
    shader->setRegisteredUniformFromMat4("mouseRotation",m_mouseGlobalTX);
    // this flag tells OpenGL to discard the data once it has passed the transform stage, this means
    // that none of it wil be drawn (RASTERIZED) remember to turn this back on once we have done this
    glEnable(GL_RASTERIZER_DISCARD);
    // redirect all draw output to the transform feedback buffer which is our buffer object matrix
    glBeginTransformFeedback(GL_POINTS);
    // now draw our array of points (now is a good time to check out the feedback.vs shader to see what
    // happens here)

    glDrawArrays(GL_POINTS, 0, m_instances);
    // now signal that we have done with the feedback buffer
    glEndTransformFeedback();
    // and re-enable rasterisation
    glDisable(GL_RASTERIZER_DISCARD);
}

void NGLScene::drawParticles()
{
    //----------------------------------------------------------------------------------------
    // DRAW INSTANCES
    //----------------------------------------------------------------------------------------------------------------------
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // now we are going to switch to our texture shader and render our boxes
    (*shader)["TextureShader"]->use();
    // set the projection matrix for our camera
    shader->setRegisteredUniformFromMat4("Projection",m_cam->getProjectionMatrix());
    // activate our vertex array object for the box
    glBindVertexArray(m_vaoID);

    // activate the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, m_tboID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_textureName);
    glPolygonMode(GL_FRONT_AND_BACK,m_polyMode);

    //glDrawArrays(GL_TRIANGLES, 0, 6, m_instances);
    glDrawArrays(GL_POINTS,0,m_instances);
}


void NGLScene::render()
{

   // Rotation based on the mouse position for our global
   // transform
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


   updateParticles();

   drawParticles();

   std::cout<<"tex "<<m_textureName<<" buffer id "<<m_tboID<<"\n";
   ++m_frames;
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   m_text->setColour(1,1,0);
   QString text=QString("Texture and Vertex Array Object %1 instances Demo %2 fps").arg(m_instances).arg(m_fps);
   m_text->renderText(10,20,text);
   text=QString("Points = %1 ").arg(m_instances);
   m_text->renderText(10,40,text);
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
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Equal : incInstances(); break;
  case Qt::Key_Minus : decInstances(); break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    renderLater();
}

void NGLScene::timerEvent( QTimerEvent *_event )
{
  if(_event->timerId() == m_fpsTimer)
    {
      if( m_timer.elapsed() > 1000.0)
      {
        m_fps=m_frames;
        m_frames=0;
        m_timer.restart();
      }
     }
      // re-draw GL
  renderNow();
}

