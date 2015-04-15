#ifndef PARTICLE_H__
#define PARTICLE_H__

#include<GL/gl.h>
#include<ngl/Vec3.h>

class emitter;

class particle
{
public:
    particle(ngl::Vec3 _pos,
             ngl::Vec3 _vel,
             GLfloat _maxlife,
             emitter &_emit
             );

    ~particle();

    void update();

private:

    GLfloat m_life;
    GLfloat m_maxLife;

    ngl::Vec3 m_position;
    ngl::Vec3 m_velocity;

    emitter* m_emitter;
};
