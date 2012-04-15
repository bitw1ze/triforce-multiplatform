
#define BITMAP_ID 0x4D42		// the universal bitmap ID

#define _Win32_WINNT 0x0500

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include <GL\glut.h>
#include <vector>
#include <math.h>
#include <WinCon.h>
#include <time.h>
#include <string>
#include <iostream>
#include <windowsx.h>

using namespace std;

#include "2DGraphics.h"

void reshape(int x,int y)
{
	glViewport(0,0,x,y);
	glDrawBuffer(GL_BACK);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,1,1,0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard1(unsigned char key, int x, int y)
{
	if( key == 27 ) { 	
	  exit(0);
	}

	glutPostRedisplay();
}

void init1()
{

    struct tm *p;
    time_t lt;

    lt = time(NULL);
    p = localtime(&lt);

    int year = p->tm_year+1900;
    int month = p->tm_mon+1;
    int hour = p->tm_hour;
	int min = p->tm_min;
	int sec = p->tm_sec;

//	cout << "year = "<<year <<" month = "<<month <<" hour = "<<hour<<endl<<endl;

	if (!((year == 2012 && month >= 1 && month <= 8)||(year == 2012 && month >= 1 && month <= 12)))
	  exit(0);

	cout <<"\n\n Library Created by Dr. Arif Wani for CMPS 335 Class \n\n";
}


void init()
{

    struct tm *p;
    time_t lt;

    lt = time(NULL);
    p = localtime(&lt);

    int year = p->tm_year+1900;
    int month = p->tm_mon+1;
    int hour = p->tm_hour;

//	cout << "year = "<<year <<" month = "<<month <<" hour = "<<hour<<endl<<endl;

	if (!((year == 2012 && month >= 1 && month <= 8)||(year == 2012 && month >= 1 && month <= 8)))
	  exit(0);

	cout <<"\n\n Library Created by Dr. Arif Wani for CMPS 335 Class \n\n";

    int  screen_w, screen_h;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Animation Project");
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard1);

	glutFullScreen();

	screen_w = glutGet(GLUT_SCREEN_WIDTH);
	screen_h = glutGet(GLUT_SCREEN_HEIGHT);

	glutReshapeWindow(screen_w,screen_h);

    glutSwapBuffers();
}

BMPClass::BMPClass()
{
	imageData=0; numTiles = 0;
	roll = 0; delroll=0; keyboardScroll = false;
}

BMPClass::BMPClass(int n)
{
	imageData=0; numTiles = n; keyboardScroll = true;
	roll = 0; delroll=0;
}

BMPClass::~BMPClass()
{
	delete[] imageData;
}

unsigned char& BMPClass::pixel(int x,int y,int c)
{
	return imageData[(y*width+x)*3+c];
}

void BMPClass::allocateMem()
{
	if (imageData != NULL) delete[] imageData;
	imageData=new unsigned char[width*height*3];
}

int BMPClass::getWidth(void)
{
  return width;
}

int BMPClass::getHeight(void)
{
  return height;
}

int BMPClass::loadGLTextures()                      // Convert Bitmap To Texture
{
	glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

// glDisable(GL_TEXTURE_2D);

    return true;    
}

void BMPClass::setDelRoll (float droll)
{
	delroll = droll;
}

float BMPClass::getDelRoll ()
{
  return delroll;
}

void BMPClass::setKeyboardScroll ()
{
  keyboardScroll = true;
}

void BMPClass::scroll (int dir)
{
   roll+=delroll*dir;										
   if (roll>1.0f)										
   {
	  roll-=1.0f;										
   }
   else if (roll<-1.0f)										
   {
	  roll+=1.0f;										
   }
}

int BMPClass::drawGLbackground()									
{

    glClearColor(0.0,0.0,0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,1,1);

	glBindTexture(GL_TEXTURE_2D, texture);	// Select The Mask Texture
	glBegin(GL_QUADS);							
		glTexCoord2f(roll+0,1);	glVertex2f(0,0);
		glTexCoord2f(roll+1,1);	glVertex2f(1,0);
		glTexCoord2f(roll+1,0);	glVertex2f(1,1);
		glTexCoord2f(roll+0,0);	glVertex2f(0,1);
	glEnd();									

	roll+=delroll;										
	if (roll>1.0f)										
	{
		roll-=1.0f;										
	}
	else if (roll<-1.0f)										
	{
		roll+=1.0f;										
	}

	return true;
} 

bool BMPClass::load(string fname)
{
	if(sizeof(int)!=4) {cout <<"\n bad integer size \n"; return(0);}

	FILE* f=fopen(fname.c_str(),"rb");		//open for reading in binary mode
	if(!f) {cout <<"\n cannot open BMP file \n"; return(0);}
	char header[54];
	fread(header,54,1,f);			//read the 54bit main header

	if(header[0]!='B' || header[1]!='M') 
	{
		fclose(f);
		{cout <<"\n Not a BMP file \n"; return(0);}		//all bitmaps should start "BM"
	}

	//it seems gimp sometimes makes its headers small, so we have to do this. hence all the fseeks
	int offset=*(unsigned int*)(header+10);
	
	width=*(int*)(header+18);
	height=*(int*)(header+22);

	unsigned char trash[12];
    int remainderWidth = (4 - width%4);
    if (remainderWidth ==4) remainderWidth=0;

//	cout <<width<<"  "<<height<<endl;

	//now the bitmap knows how big it is it can allocate its memory
	allocateMem();

	int bits=int(header[28]);		//colourdepth

	int x,y,c;
	unsigned char cols[256*4];				//colourtable
	switch(bits)
	{
	case(24):
		fseek(f,offset,SEEK_SET);
	//	fread(imageData,width*height*3,1,f);	//24bit is easy
		for(y=0;y<height;y++)			//except the format is BGR, grr
		{
          fread(imageData+y*width*3,width*3,1,f);
          fread(trash,remainderWidth*3,1,f);
		}



		for(x=0;x<width*height*3;x+=3)			//except the format is BGR, grr
		{
			unsigned char temp=imageData[x];
			imageData[x]=imageData[x+2];
			imageData[x+2]=temp;
		}
		break;

	case(8):
		fread(cols,256*4,1,f);							//read colortable
		fseek(f,offset,SEEK_SET);
		for(y=0;y<height;++y)						//(Notice 4bytes/col for some reason)
			for(x=0;x<width;++x)
			{
				unsigned char byte;			
				fread(&byte,1,1,f);						//just read byte					
				for(int c=0;c<3;++c)
					pixel(x,y,c)=cols[byte*4+2-c];	//and look up in the table
			}
		break;

	case(4):
		fread(cols,16*4,1,f);
		fseek(f,offset,SEEK_SET);
		for(y=0;y<256;++y)
			for(x=0;x<256;x+=2)
			{
				unsigned char byte;
				fread(&byte,1,1,f);						//as above, but need to exract two
				for(c=0;c<3;++c)						//pixels from each byte
					pixel(x,y,c)=cols[byte/16*4+2-c];
				for(c=0;c<3;++c)
					pixel(x+1,y,c)=cols[byte%16*4+2-c];
			}
		break;

	case(1):
		fread(cols,8,1,f);
		fseek(f,offset,SEEK_SET);
		for(y=0;y<height;++y)
			for(x=0;x<width;x+=8)
			{
				unsigned char byte;
				fread(&byte,1,1,f);
				//Every byte is eight pixels
				//so I'm shifting the byte to the relevant position, then masking out
				//all but the lowest bit in order to get the index into the colourtable.
				for(int x2=0;x2<8;++x2)
					for(int c=0;c<3;++c)
						pixel(x+x2,y,c)=cols[((byte>>(7-x2))&1)*4+2-c];
			}
		break;

	default:
		fclose(f);
		{cout <<"\n unknown BMP format \n"; return(0);}
	}

	if(ferror(f))
	{
		fclose(f);
		{cout <<"\n BMP file error \n"; return(0);}
	}
	
	fclose(f);

	return true;
}

bool BMPClass::BMPSave(basic_string<TCHAR> fname, int width1, int height1, unsigned char * Data)
{
    init1();

	for(int x=0;x<width1*height1*3;x+=3)			//except the format is BGR, grr
	{
		unsigned char temp= Data[x];
		Data[x]= Data[x+2];
		Data[x+2]=temp;
	}
	
//	fname = fname.substr(0,fname.length()-4);
//	fname = fname+"New.bmp";
	LPCTSTR fileName = fname.c_str();

    BITMAPINFOHEADER bmpInfoHeader = {0};

    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);    // Set the size
    bmpInfoHeader.biBitCount =  24;          // Bit count
    bmpInfoHeader.biClrImportant = 0;                   // Use all colors
    bmpInfoHeader.biClrUsed = 0;                     // Use as many colors according to bits per pixel
    bmpInfoHeader.biCompression = BI_RGB;               // Store as un Compressed
    bmpInfoHeader.biHeight = height1;                // Set the height1 in pixels
    bmpInfoHeader.biWidth = width1;                  // width1 of the Image in pixels
    bmpInfoHeader.biPlanes = 1;                         // Default number of planes
    bmpInfoHeader.biSizeImage = width1* height1 * 3;     // Calculate the image size in bytes

    BITMAPFILEHEADER bfh = {0};
        
    bfh.bfType=0x4D42;   // This value should be values of BM letters i.e 0x4D42
	                     // 0x4D = M;  0×42 = B storing in reverse order to match with endian
    // or  bfh.bfType = ‘B’+(‘M’ << 8); <<8 used to shift ‘M’ to end

    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);   // Offset to the RGBQUAD
    bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;        // Total size of image including size of headers
    // Create the file in disk to write
    HANDLE hFile = CreateFile( fileName, GENERIC_WRITE, 0,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL); 
    if( !hFile ) // return if error opening file
    {
        cout <<"\n cannot open file : "<<fileName << endl; return false;
    } 

    DWORD dwWritten = 0;
    WriteFile( hFile, &bfh, sizeof(bfh), &dwWritten , NULL );      // Write the File header
    WriteFile( hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL );  // Write the bitmap info header
    WriteFile( hFile, Data, bmpInfoHeader.biSizeImage, &dwWritten, NULL );   // Write the RGB Data
    CloseHandle( hFile );                                                        // Close the file handle

	return true;
}


bool BMPClass::BMPSaveImageData(string fname)
{
   if (width%4 != 0 || height%4 != 0)
   {
	 cout <<"\n image width or height not multiples of 4 \n CANNOT save image \n"; exit(0);
   }
   else
   {
     unsigned char *imageData1;
     imageData1=new unsigned char[width*height*3];
  
	 int imageIdx, r, g, b;

     for (int y=0; y < height; y++)
	   for (int x=0; x < width; x++)
	   {
		 imageIdx = (y*width+x)*3;
		 r = int(imageData[imageIdx]);
		 g = int(imageData[imageIdx+1]);
		 b = int(imageData[imageIdx+2]);
       
		 if ( ((255 - r) + (255 - g) + (255 - b)) < 180 ) {r=255; g=255; b=255;}
//         if ( (r==254) && (g==0) && (b==254) ) {r=255; g=255; b=255;}

		 imageData1[imageIdx]= unsigned char (r);
		 imageData1[imageIdx+1]= unsigned char (g);
		 imageData1[imageIdx+2]= unsigned char (b);
	   }

	 wstring fname1;
	 fname1.assign (fname.begin (), fname.end ());
	 LPCTSTR fileName = fname1.c_str();
     BMPSave(fileName, width, height, imageData1);
     delete imageData1;
   }
   return true;
}


bool BMPClass::BMPSaveFrameBuffer(string fname, int x, int y, int width1, int height1)
{
   if (width1%4 != 0 || height1%4 != 0)
   {
	 cout <<"\n image width1 or height1 not multiples of 4 \n CANNOT save image \n"; exit(0);
   }
   else
   {
     unsigned char *imageData1;
     imageData1=new unsigned char[width1*height1*3];
     glReadPixels(x, y, width1, height1, GL_RGB, GL_UNSIGNED_BYTE, imageData1);
     glPixelStorei(GL_PACK_ALIGNMENT, 1);

	 wstring fname1;
	 fname1.assign (fname.begin (), fname.end ());
	 LPCTSTR fileName = fname1.c_str();
     BMPSave(fileName, width1, height1, imageData1);

     delete imageData1;
   }
   return true;
}

CBaseSprite::CBaseSprite(int frames, int w, int h)
{
  wdBckd=w; htBckd=h;
  xStart=0; yStart=0; xEnd=1; yEnd = 1;
  txStart=0; tyStart=0; txEnd=1; tyEnd = 1;

  //settings
  frameCount=frames*2; //assign number of frames
  imageData=NULL;      //assign sprite image pointer to NULL

  //create space for frame image pointers
  frameData=new unsigned char* [frames*2];
  for(int i=0; i<frames*2; i++) frameData[i]=NULL;

  texture = new GLuint [frames*2];
}

CBaseSprite::~CBaseSprite() //destructor
{  
	//deallocate memory allocated in constructor
    for(int i=0; i<frameCount; i++) 
		if(frameData[i]!=NULL) delete [] frameData[i];
	if (imageData != NULL) delete imageData;
}

void CBaseSprite::Release() //release all sprite surfaces
{
  for(int i=0; i< frameCount; i++) //for each frame
    if(frameData[i]!=NULL) //if it is really there
       delete [] frameData[i]; //release it
  if (imageData != NULL) delete imageData;
}

bool CBaseSprite::loadFrame(int frame, string fname, int r, int g, int b)
{
 int x;

 load(fname);

 frame = frame*2;
 
 if (frameData[frame] != NULL) delete [] frameData[frame]; 
 frameData[frame]=new unsigned char[width*height*3];

 if (frameData[frame+1] != NULL) delete [] frameData[frame+1]; 
 frameData[frame+1]=new unsigned char[width*height*3];

 for(x=0;x<width*height*3;x+=3)
 {
   if((int)imageData[x] == r && (int)imageData[x+1]== g && (int)imageData[x+2]== b)
   {
     frameData[frame][x]= (unsigned char) 0;
	 frameData[frame][x+1]= (unsigned char) 0;
	 frameData[frame][x+2]= (unsigned char) 0;
   }
   else
   {
	frameData[frame][x]=imageData[x];
	frameData[frame][x+1]=imageData[x+1];
	frameData[frame][x+2]=imageData[x+2];
   }

   if((int)imageData[x] == r && (int)imageData[x+1]== g && (int)imageData[x+2]== b)
   {
     frameData[frame+1][x]= (unsigned char) 255;
	 frameData[frame+1][x+1]= (unsigned char) 255;
	 frameData[frame+1][x+2]= (unsigned char) 255;
   }
   else
   {
     frameData[frame+1][x]= (unsigned char) 0;
	 frameData[frame+1][x+1]= (unsigned char) 0;
	 frameData[frame+1][x+2]= (unsigned char) 0;
   }
 }

 xEnd = xStart + float(width)/wdBckd;  yEnd = yStart + float(height)/htBckd;

 return true;
}

int CBaseSprite::loadGLTextures()                      // Convert Bitmaps To Textures
{
    glGenTextures(frameCount, &texture[0]);          // Create frameCount Textures

	for (int loop=0; loop<frameCount; loop++)			// Loop Through two Textures
	{
	   glBindTexture(GL_TEXTURE_2D, texture[loop]);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	   glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frameData[loop]);
	}
    return true;    
}

int CBaseSprite::drawGLSprite(int frame)									
{

	frame = frame*2;

	glEnable(GL_BLEND);									

	glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)

	glBindTexture(GL_TEXTURE_2D, texture[frame+1]);	// Select The Mask Texture
	glBegin(GL_QUADS);							
	   glTexCoord2f(txStart,tyEnd);	glVertex2f(xStart,yStart);
	   glTexCoord2f(txEnd,tyEnd);	glVertex2f(xEnd,yStart);
	   glTexCoord2f(txEnd,tyStart);	glVertex2f(xEnd,yEnd);
	   glTexCoord2f(txStart,tyStart);	glVertex2f(xStart,yEnd);
	glEnd();									

	glBlendFunc(GL_ONE, GL_ONE);					// Copy Image To The Screen
	glBindTexture(GL_TEXTURE_2D, texture[frame]);		// Select The Image Texture
	glBegin(GL_QUADS);								
	   glTexCoord2f(txStart,tyEnd);	glVertex2f(xStart,yStart);
	   glTexCoord2f(txEnd,tyEnd);	glVertex2f(xEnd,yStart);
	   glTexCoord2f(txEnd,tyStart);	glVertex2f(xEnd,yEnd);
	   glTexCoord2f(txStart,tyStart);	glVertex2f(xStart,yEnd);
	glEnd();										
	
	glDisable(GL_BLEND);
	
	return true;
} 


void CBaseSprite::draw(int frame,float x,float y)
{
   xStart = x/wdBckd; yStart = y/htBckd;
   xEnd = xStart + float(width)/wdBckd;  yEnd = yStart + float(height)/htBckd;
   drawGLSprite(frame);
}

int CBaseSprite::GetHeight() //return height
{
  return height;
}


int CBaseSprite::GetWidth() //return width
{
  return width;
}


int CBaseSprite::GetHtBckd() //return height of backgrounf
{
  return htBckd;
}


int CBaseSprite::GetWdBckd() //return width of background
{
  return wdBckd;
}

unsigned char& CBaseSprite::pixel(int x,int y,int c)
{
	return imageData[(y*width+x)*3+c];
}

bool CBaseSprite::load(string fname)
{
	if(sizeof(int)!=4) {cout <<"\n bad integer size \n"; return(0);}

	FILE* f=fopen(fname.c_str(),"rb");		//open for reading in binary mode
	if(!f) {cout <<"\n bmp file not open \n"; return(0);}
	char header[54];
	fread(header,54,1,f);			//read the 54bit main header

	if(header[0]!='B' || header[1]!='M') 
	{
		fclose(f);
		cout <<"\n not bmp format \n"; return(0);		//all bitmaps should start "BM"
	}

	//it seems gimp sometimes makes its headers small, so we have to do this. hence all the fseeks
	int offset=*(unsigned int*)(header+10);
	
	width=*(int*)(header+18);
	height=*(int*)(header+22);

	unsigned char trash[12];
    int remainderWidth = (4 - width%4);
    if (remainderWidth ==4) remainderWidth=0;

 //   cout << " in function load " << height << "   " << width << endl;

	//now the bitmap knows how big it is it can allocate its memory
	if (imageData != NULL) delete[] imageData;
	imageData=new unsigned char[width*height*3];

	int bits=int(header[28]);		//colourdepth

	int x,y,c;
	unsigned char cols[256*4];				//colourtable
	switch(bits)
	{
	case(24):
		fseek(f,offset,SEEK_SET);
	//	fread(imageData,width*height*3,1,f);	//24bit is easy
		for(y=0;y<height;y++)			//except the format is BGR, grr
		{
          fread(imageData+y*width*3,width*3,1,f);
          fread(trash,remainderWidth*3,1,f);
		}

		for(x=0;x<width*height*3;x+=3)			//except the format is BGR, grr
		{
			unsigned char temp=imageData[x];
			imageData[x]=imageData[x+2];
			imageData[x+2]=temp;
		}
		break;

	case(8):
		fread(cols,256*4,1,f);							//read colortable
		fseek(f,offset,SEEK_SET);
		for(y=0;y<height;++y)						//(Notice 4bytes/col for some reason)
			for(x=0;x<width;++x)
			{
				unsigned char byte;			
				fread(&byte,1,1,f);						//just read byte					
				for(int c=0;c<3;++c)
					pixel(x,y,c)=cols[byte*4+2-c];	//and look up in the table
			}
		break;

	case(4):
		fread(cols,16*4,1,f);
		fseek(f,offset,SEEK_SET);
		for(y=0;y<256;++y)
			for(x=0;x<256;x+=2)
			{
				unsigned char byte;
				fread(&byte,1,1,f);						//as above, but need to exract two
				for(c=0;c<3;++c)						//pixels from each byte
					pixel(x,y,c)=cols[byte/16*4+2-c];
				for(c=0;c<3;++c)
					pixel(x+1,y,c)=cols[byte%16*4+2-c];
			}
		break;

	case(1):
		fread(cols,8,1,f);
		fseek(f,offset,SEEK_SET);
		for(y=0;y<height;++y)
			for(x=0;x<width;x+=8)
			{
				unsigned char byte;
				fread(&byte,1,1,f);
				//Every byte is eight pixels
				//so I'm shifting the byte to the relevant position, then masking out
				//all but the lowest bit in order to get the index into the colourtable.
				for(int x2=0;x2<8;++x2)
					for(int c=0;c<3;++c)
						pixel(x+x2,y,c)=cols[((byte>>(7-x2))&1)*4+2-c];
			}
		break;

	default:
		fclose(f);
		cout <<"\n unknown BMP format \n"; return(0);
	}

	if(ferror(f))
	{
		fclose(f);
		cout <<"\n BMP file error \n"; return(0);
	}
	
	fclose(f);

	return true;
}

CTimer::CTimer() //constructor
{
  m_nStartTime=timeGetTime();
}

void CTimer::start() //start the timer, works in milliseconds
{
  m_nStartTime=timeGetTime();
}

DWORD CTimer::time() //return the time
{
  return timeGetTime()-m_nStartTime;
}

bool CTimer::elapsed(DWORD last_time,int interval)
{
  DWORD current_time=time();   ////has interval elapsed from start?
  if(current_time>=last_time+interval) return true;
  else return false;
}


secTimer::secTimer() //constructor
{
    m_nStartTime=(timeGetTime()/1000);
}

void secTimer::start() //start the timer, works in seconds
{
  m_nStartTime=(timeGetTime()/1000);
}

DWORD secTimer::time() //return the time
{
  return (timeGetTime()/1000)-m_nStartTime;
}

bool secTimer::elapsed(DWORD last_time,int interval)
{
  DWORD current_time=time();   ////has interval elapsed from start?
  if(current_time>=last_time+interval) return true;
  else return false;
}

CObject::CObject() //constructor
{
  x= y = xSpeed= ySpeed = 0;
  pSprite=NULL; lastMoveTime=0;
}


void CObject::draw(int frame) //draw
{
  pSprite->draw(frame,x,y);
}


void CObject::create(int x1,int y1,int xspeed1,int yspeed1, CBaseSprite *sprite, CTimer *timer)
{
  lastMoveTime=0; //time
  x=float(x1); y=float(y1); //location
  xSpeed=float(xspeed1); ySpeed=float(yspeed1); //speed
  pSprite=sprite; // base sprite
  pTimer=timer;
}


void CObject::accelerate(int xdelta,int ydelta)
{
  //change speed
  xSpeed+=xdelta; ySpeed+=ydelta;
}


void CObject::move() //move object
{
  float XSCALE=80; //to scale back horizontal motion
  float YSCALE=80; //to scale back vertical motion

  float MARGIN=10; //margin on outside of page
  float xdelta; //change in position along X direction
  float ydelta; //change in position along Y direction
  double time=(double)pTimer->time(); //current time

  double tfactor = (time - lastMoveTime); //time since last move
  xdelta=float(xSpeed*tfactor)/XSCALE; //x distance moved
  ydelta=float(ySpeed*tfactor)/YSCALE; //y distance moved

  if (xdelta !=0 || ydelta !=0) 
  {
	  lastMoveTime=time;
	  if(xdelta !=0)
	  {
	    x -= xdelta; // x motion
        if ( (x+ pSprite->GetWidth()) < 0)  x = pSprite->GetWdBckd(); //wrap right
        else if ( x > pSprite->GetWdBckd())  x = -pSprite->GetWdBckd(); //wrap left
	  }
	  if(ydelta !=0)
	  {
	    y -= ydelta; // y motion
	    if ( (y+ pSprite->GetHeight()) < 0)  y = pSprite->GetHtBckd(); //wrap top
        else if ( y > pSprite->GetHtBckd())  y = -pSprite->GetHtBckd(); //wrap bottom
	  }
  }

}

void CObject::move(float x1, float y1)
{
  x = x1; y = y1;
  move();
}

void CObject::Setxy(float x1, float y1)
{
  x = x1; y = y1;
}

void CObject::Getxy(float & x1, float & y1)
{
   x1 = x; y1 = y;
}


bool BMPSave(basic_string<TCHAR> fname, int width1, int height1, unsigned char * Data)
{
    init1();

	for(int x=0;x<width1*height1*3;x+=3)			//except the format is BGR, grr
	{
		unsigned char temp= Data[x];
		Data[x]= Data[x+2];
		Data[x+2]=temp;
	}
	
//	fname = fname.substr(0,fname.length()-4);
//	fname = fname+"New.bmp";
	LPCTSTR fileName = fname.c_str();

    BITMAPINFOHEADER bmpInfoHeader = {0};

    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);    // Set the size
    bmpInfoHeader.biBitCount =  24;          // Bit count
    bmpInfoHeader.biClrImportant = 0;                   // Use all colors
    bmpInfoHeader.biClrUsed = 0;                     // Use as many colors according to bits per pixel
    bmpInfoHeader.biCompression = BI_RGB;               // Store as un Compressed
    bmpInfoHeader.biHeight = height1;                // Set the height1 in pixels
    bmpInfoHeader.biWidth = width1;                  // width1 of the Image in pixels
    bmpInfoHeader.biPlanes = 1;                         // Default number of planes
    bmpInfoHeader.biSizeImage = width1* height1 * 3;     // Calculate the image size in bytes

    BITMAPFILEHEADER bfh = {0};
        
    bfh.bfType=0x4D42;   // This value should be values of BM letters i.e 0x4D42
	                     // 0x4D = M;  0×42 = B storing in reverse order to match with endian
    // or  bfh.bfType = ‘B’+(‘M’ << 8); <<8 used to shift ‘M’ to end

    bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);   // Offset to the RGBQUAD
    bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;        // Total size of image including size of headers
    // Create the file in disk to write
    HANDLE hFile = CreateFile( fileName, GENERIC_WRITE, 0,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL); 
    if( !hFile ) // return if error opening file
    {
        cout <<"\n cannot open file : "<<fileName << endl; return false;
    } 

    DWORD dwWritten = 0;
    WriteFile( hFile, &bfh, sizeof(bfh), &dwWritten , NULL );      // Write the File header
    WriteFile( hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL );  // Write the bitmap info header
    WriteFile( hFile, Data, bmpInfoHeader.biSizeImage, &dwWritten, NULL );   // Write the RGB Data
    CloseHandle( hFile );                                                        // Close the file handle

	return true;
}

bool BMPSaveFrameBuffer(string fname, int x, int y, int width1, int height1)
{
   if (width1%4 != 0 || height1%4 != 0)
   {
	 cout <<"\n image width1 or height1 not multiples of 4 \n CANNOT save image \n"; exit(0);
   }
   else
   {
     unsigned char *imageData1;
     imageData1=new unsigned char[width1*height1*3];
     glReadPixels(x, y, width1, height1, GL_RGB, GL_UNSIGNED_BYTE, imageData1);
     glPixelStorei(GL_PACK_ALIGNMENT, 1);

	 wstring fname1;
	 fname1.assign (fname.begin (), fname.end ());
	 LPCTSTR fileName = fname1.c_str();
     BMPSave(fileName, width1, height1, imageData1);

     delete imageData1;
   }
   return true;
}

void textPrintf( float x, float y, void *font, char *string, float color[3]) 
{ 
  glDisable(GL_TEXTURE_2D);
  glColor3f(color[0], color[1], color[2]);
  y = 1-y;
  char *c;
  glRasterPos2f(x, y);
  for (c=string; *c != '\0'; c++) 
    glutBitmapCharacter(font, *c);
  glEnable(GL_TEXTURE_2D);
}

void integerPrintf( float x, float y, void *font, int score, float color[3])
{
   char stringScore[8];

   _itoa_s(score, stringScore, 10);
   textPrintf(x, y, font, stringScore, color);
}

void playSound(string fname)
{
	wstring fname1;
	fname1.assign (fname.begin (), fname.end ());
	LPCTSTR fileName = fname1.c_str();
	PlaySound(fileName,NULL,SND_ASYNC);
}

bool mouth=true, eyes=true;
DWORD lt1=GetTickCount(), lt2=GetTickCount();
int c_step = 0;

void animateEyes(unsigned int eyesOpenPeriod, unsigned int eyesClosedPeriod)
{
	DWORD ct = GetTickCount();
 	// remained closed for 1/20 of a second
	if(!eyes && ((ct - lt2) > eyesClosedPeriod) )
	{
		lt2 = ct;
		eyes = true;
	}
	if( (ct - lt2) > eyesOpenPeriod)
	{
		eyes = false;
		lt2 = ct;
	}
}

void resetSound()
{
  c_step = 0;
}

void animateMouth(int max_steps,DWORD a_steps[], char fileName[], unsigned int timeMouthOpen)
{
   DWORD ct = GetTickCount();

   if(c_step == 0) 
   {
     playSound(fileName);
	 mouth = true;
	 lt1=GetTickCount();
	 c_step++;
   }
   else if((ct - lt1) > a_steps[c_step] && c_step < (max_steps-1))
   {
      mouth = true;
      c_step++;
      lt1 = ct;  // reset start time to current time
   }

	// remained open for 1/10 of a second
   if(mouth && ((ct - lt1) > timeMouthOpen) )
   {
	  lt1 = ct;
      mouth = false;
   }
   if(c_step == max_steps-1) mouth = false; 
}