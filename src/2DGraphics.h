
#pragma once

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
    BMPClass(int w, int h, int vw, int vh);   // w, h -> full image width and height. vw, vh -> partial image width and heigh to be drawn
	~BMPClass();
	unsigned char& pixel(int x,int y,int c);
	void allocateMem();
	bool load(string fname);
    int loadGLTextures();                          // Convert Bitmap To Texture
    int drawGLbackground();
	int getViewportWidth(void);
	int getViewportHeight(void);
	void setScrollDirection(int xd, int yd);             // set direction of scroll in x and y directions
	void setDelRoll (float xdroll, float ydroll);  // set amount of scroll along x and y directions
	void getDelRoll (float & xdroll, float & ydroll); 
	void setKeyboardScrollTrue();                 // turns keyboard scroll on and auto scroll off
	void setAutoScrollTrue();                     // turns auto scroll on and keyboard scroll off
	void setScrollFalse();                        // turns both auto and keyboard scrolls off
    void scroll (int xdir, int ydir);
	void scroll ();
    bool BMPSave(basic_string<TCHAR> fname, int width1, int height1, unsigned char * Data);
    bool BMPSaveImageData(string fname);
    bool BMPSaveFrameBuffer(string fname, int x, int y, int width1, int height1);

protected:
	int width,height;     // full width and height of the background image
	int viewportWd, viewportHt;   // partial width and height of the background image to be drawn
	bool keyboardScroll, autoScroll, viewFullBackground;
	float xroll, yroll, xdelroll, ydelroll, xdir, ydir;
	float txStart, tyStart, txEnd, tyEnd;      // start and end coordinates (as a fraction of 1) of the background to be drawn 
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
	CBaseSprite *sprite;
    CObject(); //constructor
    void draw(int frame); //draw
    void create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *sprite, CTimer *timer); //create instance
    void accelerate(int xdelta,int ydelta); //change speed
    void move(); //make a move depending on time and speed
    void move(float x1, float y1); 
    void Setxy(float x1, float y1); 
    void Getxy(float & x1, float & y1);
	CBaseSprite *getSprite() const { cout << "getSprite(): " << sprite << endl; return sprite; }
	void setSprite(CBaseSprite *sprite) { pSprite = sprite; }
	void init(CBaseSprite *spr, int x1, int y1, int xspeed1,int yspeed1, CTimer *ct) { 
		create(x1, y1, xspeed1, yspeed1, spr, ct); 
		sprite = spr;
		enabled = true;
	}
	void offsetX(int _x) { x += (float)_x; }
	void offsetY(int _y) { y += (float)_y; }
	void offsetXY(int _x, int _y) { x += (float)_x; y += (float)_y; }
	int getX() { return (int)x; }
	int getY() { return (int)y; }
	void setX(int _x) { x = (float)x; }
	void setY(int _y) { y = (float)y; }
	bool enabled;
};

bool BMPSaveFrameBuffer(string fname, int x, int y, int width1, int height1);
void textPrintf( float x, float y, void *font, char *string, float color[3]) ;
void integerPrintf( float x, float y, void *font,int score, float color[3]) ;
void playSound(string fname);
void animateEyes(unsigned int eyesOpenPeriod, unsigned int eyesClosedPeriod);
void animateMouth(int max_steps,DWORD a_steps[], char fileName[], unsigned int timeMouthOpen);

#endif