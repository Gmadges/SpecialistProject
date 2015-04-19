#ifndef NGLSCENE_H__
#define NGLSCENE_H__
#include "OpenGLWindow.h"
#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Text.h>
#include <QTime>

#include<ngl/Vec3.h>

//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

struct Particle
{
    float Type;
    ngl::Vec3 Pos;
    ngl::Vec3 Vel;
    float LifetimeMillis;
};


class NGLScene : public OpenGLWindow
{
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene(QWindow *_parent=0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initialize();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void render();

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief update the number of instances to draw
    //----------------------------------------------------------------------------------------------------------------------
    void incInstances();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief decrease the number of instances to draw
    //----------------------------------------------------------------------------------------------------------------------
    void decInstances();

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the x rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the y rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the Right mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the global mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Camera
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Camera *m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief transformation stack for the gl transformations etc
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Transformation m_transform;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief class for text rendering
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Text *m_text;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the texture ID for the box texture
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_textureName;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief polygon draw mode
    //----------------------------------------------------------------------------------------------------------------------
    GLenum m_polyMode;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate is the number of instances have increased and if we need to update the point buffer
    //----------------------------------------------------------------------------------------------------------------------
    bool m_updateBuffer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag for the fps timer
    //----------------------------------------------------------------------------------------------------------------------
    int m_fpsTimer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the fps to draw
    //----------------------------------------------------------------------------------------------------------------------
    int m_fps;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief number of frames for the fps counter
    //----------------------------------------------------------------------------------------------------------------------
    int m_frames;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief timer for re-draw
    //----------------------------------------------------------------------------------------------------------------------
    QTime m_timer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief number of instances per block of the UBO in shader
    //----------------------------------------------------------------------------------------------------------------------
    GLint m_instancesPerBlock;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief pointer to the VAO for the point data
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_dataID;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief id for the Matrix data created from the transform feedback in the shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_matrixID;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief VAO id for our box
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_vaoID;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Texture buffer id for our box
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_tboID;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief number of instances to draw
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_instances;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load transform matrices to the shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when the window is re-sized
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void resizeEvent(QResizeEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event );

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief create a cube and stuff it into a VBO on the GPU
    /// @param[in] _scale a scale factor for the unit vertices
    //----------------------------------------------------------------------------------------------------------------------
   void createCube(GLfloat _scale);
   //----------------------------------------------------------------------------------------------------------------------
   /// @brief load a texture from a QImage
   //----------------------------------------------------------------------------------------------------------------------
    void loadTexture();
    void createDataPoints();
    void timerEvent(QTimerEvent *);

    void particleSystemInit();

    GLuint m_particleBuffer[2];
    GLuint m_transformFeedback[2];

};



#endif
