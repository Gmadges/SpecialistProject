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

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>


#include<QGLWidget>

struct Particle
{
    ngl::Vec3 Pos;
    ngl::Vec3 Vel;
    float LifetimeMillis;
};

/// @brief the increment for x/y translation with mouse movement
const static float INCREMENT=0.01;

/// @brief the increment for the wheel zoom
const static float ZOOM=5.0;


const static unsigned int maxinstances=10000;

void NGLScene::incInstances()
{
  m_instances+=10000;
  if(m_instances>maxinstances)
    m_instances=maxinstances;
  m_updateBuffer=true;
}

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

    shader->createShaderProgram("ParticleShader");

    shader->attachShader("ParticleVertex",ngl::VERTEX);
    shader->attachShader("ParticleFragment",ngl::FRAGMENT);
    shader->attachShader("ParticleGeometry",ngl::GEOMETRY);


    shader->loadShaderSource("ParticleVertex","shaders/drawVertex.glsl");
    shader->loadShaderSource("ParticleFragment","shaders/drawFragment.glsl");
    shader->loadShaderSource("ParticleGeometry","shaders/drawGeometry.glsl");

    shader->compileShader("ParticleVertex");
    shader->compileShader("ParticleFragment");
    shader->compileShader("ParticleGeometry");


    shader->attachShaderToProgram("ParticleShader","ParticleVertex");
    shader->attachShaderToProgram("ParticleShader","ParticleFragment");
    shader->attachShaderToProgram("ParticleShader","ParticleGeometry");

    // link
    shader->linkProgramObject("ParticleShader");
    shader->use("ParticleShader");

    // register the uniforms for later uses
    shader->autoRegisterUniforms("ParticleShader");
}

void NGLScene::particleSystemInit()
{
    Particle Particles[maxinstances];

    ngl::Random *rng = ngl::Random::instance();

    //create particle data.
    for(unsigned int i = 0; i < maxinstances; i++)
    {
        Particles[i].Pos = ngl::Vec3(0,0,0);
        Particles[i].Vel = ngl::Vec3(0.0f, 100.0f, 0.0f);
        Particles[i].LifetimeMillis = 0.0f;
    }
    
    // Create VBO for input on even-numbered frames and output on odd-numbered frames:
    glGenBuffers(1, &m_particleBufferA);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleBufferA);
    glBufferData(GL_ARRAY_BUFFER, maxinstances*sizeof(Particle), Particles, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create VBO for output on even-numbered frames and input on odd-numbered frames:
    glGenBuffers(1, &m_particleBufferB);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleBufferB);
    glBufferData(GL_ARRAY_BUFFER, maxinstances*sizeof(Particle), Particles, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // now to load the shader and set the values
    // grab an instance of shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    //initialise feedback shader
    shader->createShaderProgram("updateParticle");
    shader->attachShader("updateFeedback",ngl::VERTEX);
    shader->loadShaderSource("updateFeedback","shaders/updateFeedback.glsl");
    shader->compileShader("updateFeedback");
    shader->attachShaderToProgram("updateParticle","updateFeedback");

    const GLchar* Varyings[3] = { "Position0",
                                  "Velocity0",
                                  "Age0"    };


    GLuint id=shader->getProgramID("updateParticle");

    glTransformFeedbackVaryings(id, 3, Varyings, GL_INTERLEAVED_ATTRIBS);

    // now link the shader
    shader->linkProgramObject("updateParticle");
    shader->use("updateParticle");
    // register the uniforms
    shader->autoRegisterUniforms("updateParticle");
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

  Particle Particles[maxinstances];

  ngl::Random *rng = ngl::Random::instance();

  /*create particle data.
  for(unsigned int i = 0; i < maxinstances; i++)
  {
      Particles[i].Pos = rng->getRandomPoint(50,50,50);
      Particles[i].Vel = ngl::Vec3(0.0f, 100.0f, 0.0f);
      Particles[i].LifetimeMillis = 0.0f;
  }

  // Create VBO for input on even-numbered frames and output on odd-numbered frames:
  glGenBuffers(1, &m_particleBufferA);
  glBindBuffer(GL_ARRAY_BUFFER, m_particleBufferA);
  glBufferData(GL_ARRAY_BUFFER, maxinstances*sizeof(Particle), Particles, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  */

  glGenVertexArrays(1,&m_dataID);
  // bind the array
  glBindVertexArray(m_dataID);
  // allocate space for the vec3 for each point
  ngl::Vec3 *data = new ngl::Vec3[maxinstances];
  // in this case create a sort of supertorus distribution of points
  // based on a random point

  ngl::Vec3 p;
  for(unsigned int i=0; i<maxinstances; ++i)
  {
      p=rng->getRandomPoint(50,50,50);
      data[i].set(p.m_x,p.m_y,p.m_z);
  }
  // now store this buffer data for later.
  glBufferData(GL_ARRAY_BUFFER, maxinstances * sizeof(ngl::Vec3), data, GL_STATIC_DRAW);
  // attribute 0 is the inPos in our shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


  InitDrawingShaders();

  //particleSystemInit();

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
  ngl::Mat3 normalMatrix;
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

    (*shader)["updateParticle"]->use();
    // if the number of instances have changed re-bind the buffer to the correct size

    //shader->setRegisteredUniform1i("DeltaTime", m_timer.elapsed());
    //shader->setRegisteredUniform1i("Time", m_timer.elapsed());
    shader->setRegisteredUniform1i("Time", 1);

    // set the view for the camera
    glEnable(GL_RASTERIZER_DISCARD);

    //bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_particleBufferA);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Particle),0); // position
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)12); // velocity
    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)24); // lifetime

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBufferB);

    // redirect all draw output to the transform feedback buffer which is our buffer object matrix
    glBeginTransformFeedback(GL_POINTS);

    glDrawArrays(GL_POINTS, 0, m_instances);

    glEndTransformFeedback();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    std::swap(m_particleBufferA, m_particleBufferB);

    // and re-enable rasterisation
    glDisable(GL_RASTERIZER_DISCARD);
}

void NGLScene::drawParticles()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();

    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // now we are going to switch to our texture shader and render our boxes
    (*shader)["ParticleShader"]->use();

    // data for the shader
    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat4 M;
    M=m_transform.getMatrix()*m_mouseGlobalTX;
    MV=  M*m_cam->getViewMatrix();
    MVP= M*m_cam->getVPMatrix();

    // disgusting method for getting cam position.
    glm::mat3 rotMat;
    rotMat[0][0] = MVP.m_m[0][0];
    rotMat[0][1] = MVP.m_m[0][1];
    rotMat[0][2] = MVP.m_m[0][2];
    rotMat[1][0] = MVP.m_m[1][0];
    rotMat[1][1] = MVP.m_m[1][1];
    rotMat[1][2] = MVP.m_m[1][2];
    rotMat[2][0] = MVP.m_m[2][0];
    rotMat[2][1] = MVP.m_m[2][1];
    rotMat[2][2] = MVP.m_m[2][2];

    glm::vec3 d(MVP.m_m[3][0],
                MVP.m_m[3][1],
                MVP.m_m[3][3]);

    glm::vec3 tmp = -d * rotMat;

    shader->setRegisteredUniformFromMat4("ViewProjection",MVP);

    shader->setRegisteredUniformVec3("camPosition", ngl::Vec3(tmp.x, tmp.y, tmp.z));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_textureName);
    glPolygonMode(GL_FRONT_AND_BACK,m_polyMode);

    //glBindBuffer(GL_ARRAY_BUFFER, m_particleBufferA);
    glBindBuffer(GL_ARRAY_BUFFER, m_dataID);

    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ngl::Vec3), 0); // position

    glDrawArrays(GL_POINTS, 0, m_instances);

    glDisableVertexAttribArray(0);
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

   //updateParticles();

   drawParticles();

   ++m_frames;
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   m_text->setColour(1,1,0);
   QString text=QString("Texture and Vertex Array Object %1 instances Demo %2 fps").arg(m_instances).arg(m_fps);
   m_text->renderText(10,20,text);
   text=QString("Points = %1 ").arg(m_instances);
   m_text->renderText(10,40,text);
}

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

