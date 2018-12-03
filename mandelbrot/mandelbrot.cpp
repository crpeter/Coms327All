#include <stdio.h>
#include <math.h>

#include <assert.h>
#include <stdio.h>

#include "pbm.h"

struct char_rgb {
  unsigned char r, g, b;
};

struct char_rgb c[1000][1000];

int main() {
  int x,y;
  int maxX = 1000;
  int maxY = 1000;

  double Cx,Cy;

  double pixelWidth= 4.0/maxX;
  double pixelHeight= 4.0/maxY;

  double newX, newY;
  double x2, y2;
  
  double xJulia = -0.4;
  double yJulia = 0.6;

  int i;
  
  for(y=0; y<maxY; y++) {
    Cy = -2.0 + y*pixelHeight;
    if (abs(Cy) < pixelHeight/2) {
      Cy=0.0;
    }
    
    for(x=0; x<maxX; x++) {
      Cx = -2.0 + x*pixelWidth;
      newX = 0.0;
      newY = 0.0;
      x2 = newX*newX;
      y2 = newY*newY;
      
      i = 0;
      while (i < 500 && (x2+y2)<4) {
        newY = 2*newX*newY + Cy;
        newX = x2-y2 + Cx;
        x2 = newX*newX;
        y2 = newY*newY;
        i++;
      }
      
      if (i==500) {
        c[y][x].r = 0;
        c[y][x].g = 0;
        c[y][x].b = 0;
      } else {
        c[y][x].r = 255 % (i+55);
        c[y][x].g = 255 % (i+100);
        c[y][x].b = 255 % (i+155);
      }
    }
  }
  
  write_pbm("c.ppm", c, 1000, 1000, 3, 255);
  return 0;
}

