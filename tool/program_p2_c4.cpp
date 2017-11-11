#include "program_p2_c4.h"

PROGRAM_P2_C4::PROGRAM_P2_C4()
{
    _position   =   -1;
    _color      =   -1;
    _MVP        =   -1;
}

bool PROGRAM_P2_C4::initialize()
{
    //!使用（varying），是表示需要在（vs）和（ps）之间进行传递的变量，把需要传递的变量赋值，并重复使用（varying）语句即可
    const char* vs  =
    {
        "precision lowp float; "
        "uniform   mat4 _MVP;"
        "attribute vec2 _position;"
        "attribute vec4 _color;"
        "varying   vec4 _outColor;"
        "void main()"
        "{"
        "   vec4    pos =   vec4(_position,0,1);"
        "   _outColor   =   _color;"
        "   gl_Position =   _MVP * pos;"
        "}"
    };
    const char* ps  =
    {
        "precision  lowp float; "
        "varying    vec4 _outColor;"
        "void main()"
        "{"
        "   gl_FragColor   =   _outColor;"
        "}"
    };

    bool    res =   createProgram(vs,ps);
    if(res)
    {
        _position   =   glGetAttribLocation(_programId,"_position");
        _color      =   glGetAttribLocation(_programId,"_color");
        _MVP        =   glGetUniformLocation(_programId,"_MVP");
    }
    return  res;
}

void PROGRAM_P2_C4::begin()
{
    glUseProgram(_programId);
    //!在显卡里面使用的局部变量，在使用时是需要进行使能和关闭的
    glEnableVertexAttribArray(_position);       
    glEnableVertexAttribArray(_color);
}

void PROGRAM_P2_C4::end()
{
    glDisableVertexAttribArray(_position);
    glDisableVertexAttribArray(_color);
    glUseProgram(0);
}
