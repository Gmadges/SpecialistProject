#ifndef EMITTER_H__
#define EMITTER_H__

#include<vector>

class particle;


class emitter
{
public:
    emitter();
    ~emitter();


private:

    std::vector<particle> m_particles;


};
