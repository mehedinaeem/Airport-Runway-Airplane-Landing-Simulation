#ifndef AIRPORT_GRAPHICS_DECLARATIONS_H
#define AIRPORT_GRAPHICS_DECLARATIONS_H
// Compile-only declarations for checking C++ on hosts without WinBGIm.
// This is NOT a graphics implementation and is NEVER used in the application build.
constexpr int SOLID_FILL=1, DEFAULT_FONT=0, HORIZ_DIR=0;
inline int COLOR(int r,int g,int b) { return 0x03000000 | r | (g<<8) | (b<<16); }
int initwindow(int,int,const char* ="Windows BGI",int=-1,int=-1,bool=false,bool=true);
void setfillstyle(int,int);
void bar(int,int,int,int);
void setcolor(int);
void line(int,int,int,int);
void fillellipse(int,int,int,int);
void fillpoly(int,int*);
void settextstyle(int,int,int);
void setbkcolor(int);
void outtextxy(int,int,char*);
void putpixel(int,int,int);
int kbhit();
int getch();
void setactivepage(int);
void setvisualpage(int);
void closegraph(int=-1);
#endif
