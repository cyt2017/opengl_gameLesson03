#include "qgleswidget.h"

QGLESWIDGET::QGLESWIDGET(QWidget *parent) : QWidget(parent)
{

}

QGLESWIDGET::~QGLESWIDGET()
{
    destroyOpenGLES20();
}

bool QGLESWIDGET::init_QGW()
{
    if(!initOpenGLES20())
    {
        return false;
    }
    _shader.initialize();
}

bool QGLESWIDGET::initOpenGLES20()
{
    const EGLint attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE,24,
        EGL_NONE
    };
    EGLint 	format(0);
    EGLint	numConfigs(0);
    EGLint  major;
    EGLint  minor;

    //! 1
    _display	    =	eglGetDisplay(EGL_DEFAULT_DISPLAY);

    //! 2init
    eglInitialize(_display, &major, &minor);

    //! 3
    eglChooseConfig(_display, attribs, &_config, 1, &numConfigs);

    eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &format);
    //!!! 4 使opengl与qt的窗口进行绑定<this->winId()>
    _surface	    = 	eglCreateWindowSurface(_display, _config, this->winId(), NULL);

    //! 5
    EGLint attr[]   =   { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    _context 	    = 	eglCreateContext(_display, _config, 0, attr);
    //! 6
    if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE)
    {
        return false;
    }

    eglQuerySurface(_display, _surface, EGL_WIDTH,  &_width);
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &_height);

    return  true;
}

void QGLESWIDGET::destroyOpenGLES20()
{
    if (_display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (_context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(_display, _context);
        }
        if (_surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(_display, _surface);
        }
        eglTerminate(_display);
    }
    _display    =   EGL_NO_DISPLAY;
    _context    =   EGL_NO_CONTEXT;
    _surface    =   EGL_NO_SURFACE;//asdsafsaf
}

void QGLESWIDGET::render()
{
    //! 清空缓冲区
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //! 视口，在Windows窗口指定的位置和大小上绘制OpenGL内容
    glViewport(0,0,_width,_height);

    //! 创建一个投影矩阵
    //当图像显示出来与坐标的位置出现导致现象时，可以改变投影矩阵的坐标，使之对齐
    CELL::matrix4   screenProj  =   CELL::ortho<float>(0,float(_width),float(_height),0,-100.0f,100);
    _shader.begin();
    {
        float   x   =   100;
        float   y   =   100;
        float   w   =   100;
        float   h   =   100;
        Vertex  vect[]   =
        {
            CELL::float2(x,y),          CELL::Rgba4Byte(255,0,0,255),
            CELL::float2(x + w,y),      CELL::Rgba4Byte(0,255,0,255),
            CELL::float2(y,y + h),      CELL::Rgba4Byte(0,0,255,255),
            CELL::float2(x + w, y + h), CELL::Rgba4Byte(255,255,255,255),
        };

        //!使用四元素(实现缩放、平移、旋转)，使程序速度更快，主要是减少了矩阵之间的加和乘
        static float inc = 1;
        inc +=  1;
        CELL::quaternion    quat    =   CELL::angleAxis(inc,CELL::float3(0,0,1));
        CELL::matrix4       matModel=   CELL::makeTransform(CELL::float3(0,0,0),CELL::float3(1,1,1),quat);
        CELL::matrix4       matMVP  =   screenProj * matModel;

        glUniformMatrix4fv(_shader._MVP, 1, false, matMVP.data());
#if 0
        glVertexAttribPointer(_shader._position,2,GL_FLOAT,false,sizeof(Vertex),&(vect[0].vertex));
        glVertexAttribPointer(_shader._color,4,GL_UNSIGNED_BYTE,true,sizeof(Vertex),&(vect[0].color));
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
#else
        //!实现画圆
        Vertex cricle[362];
        float x1 = 200.0f;
        float y1 = 200.0f;
        float r = 100.f;

        cricle[0].vertex = CELL::float2(x1,y1);
        cricle[0].color  = CELL::Rgba4Byte(255,0,0);
        for(size_t i=1;i<362;i++)
        {
            float xx = x1 + r*cos(i*PI/180.0f);
            float yy = y1 + r*sin(i*PI/180.0f);
            cricle[i].vertex = CELL::float2(xx,yy);
        }

        glVertexAttribPointer(_shader._position,2,GL_FLOAT,false,sizeof(Vertex),cricle);
        glVertexAttribPointer(_shader._color,4,GL_UNSIGNED_BYTE,true,sizeof(Vertex),&(cricle[0].color));
        glDrawArrays(GL_TRIANGLE_FAN,0,362);
#endif
    }
    _shader.end();
}

void QGLESWIDGET::drawImage()
{
    render();
    eglSwapBuffers(_display,_surface);
}
