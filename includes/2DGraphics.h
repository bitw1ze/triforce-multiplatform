
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <windowsx.h>

#include "glut.h"

using namespace std;

class BMPClass
{
public:
	BMPClass();
    BMPClass(int n);
	~BMPClass();
	unsigned char& pixel(int x,int y,int c);
	void allocateMem();
	bool load(string fname);
    int loadGLTextures();                     // Convert Bitmap To Texture
    int drawGLbackground();
	int getWidth(void);
	int getHeight(void);
	void setDelRoll (float droll);
	float getDelRoll ();
    void scroll (int dir);
    void setKeyboardScroll ();
    bool BMPSave(basic_string<TCHAR> fname, int width1, int height1, unsigned char * Data);
    bool BMPSaveImageData(string fname);
    bool BMPSaveFrameBuffer(string fname, int x, int y, int width1, int height1);

protected:
	int width,height;
	int numTiles, wdTiledBckd, htTiledBckd;
	bool keyboardScroll;
	float roll, delroll;
	unsigned char* imageData;
	unsigned int	texture;
};
	

class CBaseSprite //simplest sprite
{
  protected:
    unsigned char **frameData; //frame images
    unsigned char *imageData; //sprite image
    int frameCount; //how many frames used in sprite
	int wdBckd, htBckd;  // width and height of background image
    int width,height; //dimensions of sprite images
	float xStart, yStart, xEnd, yEnd;      // object coordinates for texture
	float txStart, tyStart, txEnd, tyEnd;  // texture coordinates for texture
	GLuint	*texture;			// Storage Textures
  public:
    CBaseSprite(int frames, int w, int h); //constructor
    ~CBaseSprite(); //destructor
	bool loadFrame(int frame, string fname, int r, int g, int b);
	bool load(string fname);
    unsigned char& pixel(int x,int y,int c);
    virtual void draw(int frame,float x,float y); //draw sprite
    void Release(); //release direct draw surfaces
    int GetHeight(); //return height
    int GetWidth(); //return width
    int GetHtBckd(); //return backgrounf height
    int GetWdBckd(); //return background width
    int loadGLTextures();               // Convert Bitmaps To Textures
    int drawGLSprite(int frame);	
};


class CTimer{ //game timer class using ms
  protected:
    DWORD m_nStartTime; //time in ms that timer was started
  public:
    CTimer(); //constructor
    virtual void start(); //start the timer
    virtual DWORD time(); //return the time in ms
    bool elapsed(DWORD last_time,int interval);   //has interval ms elapsed since start?
};

class secTimer{ //game timer class using seconds
  protected:
    DWORD m_nStartTime; //time in sec that timer was started
  public:
    secTimer(); //constructor
    virtual void start(); //start the timer
    virtual DWORD time(); //return the time in sec
    bool elapsed(DWORD last_time,int interval);   //has interval sec elapsed since start?
};

class CObject //class for a moving object
{
  private:
    float x,y; //current location
    float xSpeed, ySpeed; //current speed
    double lastMoveTime; //last time the object moved
    CBaseSprite *pSprite; //pointer to sprite
	CTimer *pTimer;
  public:
    CObject(); //constructor
    void draw(int frame); //draw
    void create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *sprite, CTimer *timer); //create instance
    void accelerate(int xdelta,int ydelta); //change speed
    void move(); //make a move depending on time and speed
    void move(float x1, float y1); 
    void Setxy(float x1, float y1); 
    void Getxy(float & x1, float & y1);
};

bool BMPSaveFrameBuffer(string fname, int x, int y, int width1, int height1);
void textPrintf( float x, float y, void *font, char *string, float color[3]) ;
void integerPrintf( float x, float y, void *font,int score, float color[3]) ;
void playSound(string fname);
void animateEyes(unsigned int eyesOpenPeriod, unsigned int eyesClosedPeriod);
void animateMouth(int max_steps,DWORD a_steps[], char fileName[], unsigned int timeMouthOpen);

#endif