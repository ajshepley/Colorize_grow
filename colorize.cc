/**
 * CPSC 4850 - Digital Image Processing
 * Final Project
 * This program colorizes images based on
 * the organic growth of a color "profile" image submitted.
 * 
 * Date: November 27th, 2012
 * Author: Adam Shepley
 * 
 */

#ifndef M_PI
#define M_PI atan(1)*4
#endif


#ifndef DEBUG
#define DEBUG true
#endif

#define BLACK_MARGIN 150	 //what margine do we consider R+G+B to be black?
#define FINAL_INVERT true //do we want to invert back to original greyscale colors?
//the mask flips them.


#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <math.h>
#include <string.h>
#include <sstream>


#include <dirent.h>

#include "pam_assistant.h"

using namespace std;
  
tuple** ColorToGrey(tuple** inputImage, const pam &inpam, pam& greyPam);
tuple** GrowColors(tuple** color_profile_base, pam& baseColorPam);


int main(int argc, char *argv[])
{
  
  /*
   * Command inputs: argv[0] is program call
   * 		     argv[1] is input greyscale pgm name
   * 		     argv[2] is the markup color filename
   * 		     argv[3] is the output color filename
   */
  
  /* structures for input image */
  pam inpam;
  pam colorInPam;
  pam outpam;
  /* a dynamic two-dimensional array to store the pixels...note that
     pnm uses a tuple (for color images with multiple planes) for
     each pixel.  For PGM files it will only be one plane. */
  tuple **input_greyscale_array, **input_color_array, **color_output_array;

  /* initializes the library */
  pm_init(argv[0], 0);

  /* read the image */
  input_greyscale_array = read_image(argv[1], inpam);  
  input_color_array = read_image(argv[2], colorInPam);  

  outpam = colorInPam;
  tuple** finished_profile = GrowColors(input_color_array, colorInPam);
//  finished_profile = GrowColors(finished_profile, colorInPam);
  
  write_image((char*)string("color_profile.pnm").c_str(), outpam, finished_profile);

  /* clean up */
 //pnm_freepamarray(input_array, &inpam);
//  pnm_freepamarray(output_array, &outpam);
  
  stringstream callString, callStr;

  // Finalize the callstring for applying our colormask.
  callStr << "pnmcomp -alpha=" << argv[1]; 
  if(FINAL_INVERT)
    callStr << " -invert";
  callStr << " color_profile.pnm " << argv[1] << " " << argv[3];
  //pnmcomp -alpha=lenna.pgm -invert color_profile.pnm lenna.pgm fullcolor.pnm
  
  system(callStr.str().c_str());
  
  cerr << "Finished processing. Final file is " << argv[3] << endl;
  return 0;
}

int blackInImage(tuple** array, int width, int height)
{
  int blackPixelCount = 0;
  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      if((array[y][x][0] + array[y][x][1] + array[y][x][2]) < BLACK_MARGIN)
      {
	blackPixelCount++;
      }
    }
  }
  
  return blackPixelCount;
  
}

tuple** GrowColors(tuple** color_profile_base, pam& baseColorPam)
{
  pam newpam = baseColorPam;
  tuple** newColorProfile = pnm_allocpamarray(&newpam);
  int width = newpam.width, height = newpam.height;
  
  
  int blackPixelCount = 0;
  vector <int> blackPixelX;
  vector<int> blackPixelY;
  
  // Count the black pixels.
  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      if((color_profile_base[y][x][0] + color_profile_base[y][x][1] + color_profile_base[y][x][2]) < BLACK_MARGIN)
      {
	blackPixelCount++;
	blackPixelX.push_back(x);
	blackPixelY.push_back(y);
      }
      newColorProfile[y][x][0] = color_profile_base[y][x][0];
      newColorProfile[y][x][1] = color_profile_base[y][x][1];
      newColorProfile[y][x][2] = color_profile_base[y][x][2];
    }
  }
  /*
  //basically, keep going until we color everything.
  while(blackPixelCount > 0)
  {
    for(int i = 0; i < blackPixelCount; i++)
    {
      bool top=false, bottom=false, left=false, right=false;
      if(blackPixelX.at(i) == 0)
      {
	left = true;
	right = false;
      }
      else if(blackPixelX.at(i) == width - 1)
      {
	right = true;
	left = false;
      }
      else
      {
	left = false;
	right = false;
      }
      
      if(blackPixelY.at(i) == 0)
      {
	top = true;
	bottom = false;
      }
      else if(blackPixelY.at(i) == height - 1)
      {
	bottom = true;
	top = false;
      }
      else
      {
	bottom = false;
	top = false;
      }
      
      if(top)
      {
	if(left)
      
     
	
    }
  }*/
  tuple** previous = pnm_allocpamarray(&newpam);
  while(blackInImage(newColorProfile, width, height) > 0)
  {
    pam temppam = newpam;
    
    for(int y = 0; y < height; y++)
    {
      for(int x = 0; x < width; x++)
      {
	previous[y][x][0] = newColorProfile[y][x][0];
	previous[y][x][1] = newColorProfile[y][x][1];
	previous[y][x][2] = newColorProfile[y][x][2];
      }
    }
    
    bool top = false, bottom = false, left = false, right = false;
    //Pass over the image, check each pixel; if a color, check neighbors; if they aren't colored, color them.
    static int count = 1;
   // cerr << "WHILE ITER " << count << endl;
    for(int y = 0; y < height; y++)
    {
      if(y == 0)
      {
	top = true;
	bottom = false;
      }
      else if(y == height-1)
      {
	bottom = true;
	top = false;
      }
      else
      {
	bottom = false;
	top = false;
      }
      for(int x = 0; x < width; x++)
      {
	//bounds establishment
	if(x == 0)
	{
	  left = true;
	  right = false;
	}
	else if(x == width - 1)
	{
	  left = false;
	  right = true;
	}
	else
	{
	  left = false;
	  right = false;
	}
		
	
      
	//if we've found a color
	if((previous[y][x][0] + previous[y][x][1] + previous[y][x][2]) > BLACK_MARGIN)
	{
	  if(!top) //look up
	  {
	    if(!left) //look up-left
	    { 
	      if((previous[y-1][x-1][0] + previous[y-1][x-1][1] + previous[y-1][x-1][2]) < BLACK_MARGIN)
	      {
		//cerr << "WHAT" << endl;
		newColorProfile[y-1][x-1][0] = newColorProfile[y][x][0];
		newColorProfile[y-1][x-1][1] = newColorProfile[y][x][1];
		newColorProfile[y-1][x-1][2] = newColorProfile[y][x][2];
		
		blackPixelCount--;
	      }
	    }
	    if(!right) //look up-right
	      if((previous[y-1][x+1][0] + previous[y-1][x+1][1] + previous[y-1][x+1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y-1][x+1][0] = newColorProfile[y][x][0];
		newColorProfile[y-1][x+1][1] = newColorProfile[y][x][1];
		newColorProfile[y-1][x+1][2] = newColorProfile[y][x][2];
		blackPixelCount--;
	      }
	    //look up mid
	    if((previous[y-1][x][0] + previous[y-1][x][1] + previous[y-1][x][2]) < BLACK_MARGIN)
	    {
	      newColorProfile[y-1][x][0] = newColorProfile[y][x][0];
	      newColorProfile[y-1][x][1] = newColorProfile[y][x][1];
	      newColorProfile[y-1][x][2] = newColorProfile[y][x][2];
	      blackPixelCount--;
	    }
	  }
	  
	  if(!bottom)
	  {
	    if(!left) //look down-left
	      if((previous[y+1][x-1][0] + previous[y+1][x-1][1] + previous[y+1][x-1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y+1][x-1][0] = newColorProfile[y][x][0];
		newColorProfile[y+1][x-1][1] = newColorProfile[y][x][1];
		newColorProfile[y+1][x-1][2] = newColorProfile[y][x][2];
		blackPixelCount--;
	      }
	      
	    if(!right) //look down-right
	      if((previous[y+1][x+1][0] + previous[y+1][x+1][1] + previous[y+1][x+1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y+1][x+1][0] = newColorProfile[y][x][0];
		newColorProfile[y+1][x+1][1] = newColorProfile[y][x][1];
		newColorProfile[y+1][x+1][2] = newColorProfile[y][x][2];
		blackPixelCount--;
	      }
	      
	    //look down mid
	    if((previous[y+1][x][0] + previous[y+1][x][1] + previous[y+1][x][2]) < BLACK_MARGIN)
	    {
	      newColorProfile[y+1][x][0] = newColorProfile[y][x][0];
	      newColorProfile[y+1][x][1] = newColorProfile[y][x][1];
	      newColorProfile[y+1][x][2] = newColorProfile[y][x][2];
	      blackPixelCount--;
	    }
	  }
	  
	  if(!left)
	  {
	    if(!top) //look up-left
	      if((previous[y-1][x-1][0] + previous[y-1][x-1][1] + previous[y-1][x-1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y-1][x-1][0] = newColorProfile[y][x][0];
		newColorProfile[y-1][x-1][1] = newColorProfile[y][x][1];
		newColorProfile[y-1][x-1][2] = newColorProfile[y][x][2];
	      }
	      
	    if(!bottom) //look down-left
	      if((previous[y+1][x-1][0] + previous[y+1][x-1][1] + previous[y+1][x-1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y+1][x-1][0] = newColorProfile[y][x][0];
		newColorProfile[y+1][x-1][1] = newColorProfile[y][x][1];
		newColorProfile[y+1][x-1][2] = newColorProfile[y][x][2];
	      }
	      
	    //look left-mid
	    if((previous[y][x-1][0] + previous[y][x-1][1] + previous[y][x-1][2]) < BLACK_MARGIN)
	    {
	      newColorProfile[y][x-1][0] = newColorProfile[y][x][0];
	      newColorProfile[y][x-1][1] = newColorProfile[y][x][1];
	      newColorProfile[y][x-1][2] = newColorProfile[y][x][2];
	      blackPixelCount--;
	    }
	  }
	  
	  if(!right)
	  {
	    if(!top) //look up-right
	      if((previous[y-1][x+1][0] + previous[y-1][x+1][1] + previous[y-1][x+1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y-1][x+1][0] = newColorProfile[y][x][0];
		newColorProfile[y-1][x+1][1] = newColorProfile[y][x][1];
		newColorProfile[y-1][x+1][2] = newColorProfile[y][x][2];
	      }
	      
	    if(!bottom) //look down-right
	      if((previous[y+1][x+1][0] + previous[y+1][x+1][1] + previous[y+1][x+1][2]) < BLACK_MARGIN)
	      {
		newColorProfile[y+1][x+1][0] = newColorProfile[y][x][0];
		newColorProfile[y+1][x+1][1] = newColorProfile[y][x][1];
		newColorProfile[y+1][x+1][2] = newColorProfile[y][x][2];
	      }
	      //look right
	    if((previous[y][x+1][0] + previous[y][x+1][1] + previous[y][x+1][2]) < BLACK_MARGIN)
	    {
	      newColorProfile[y][x+1][0] = newColorProfile[y][x][0];
	      newColorProfile[y][x+1][1] = newColorProfile[y][x][1];
	      newColorProfile[y][x+1][2] = newColorProfile[y][x][2];
	      blackPixelCount--;
	    }
	  }
	  
	}
      
      
      }//end of x
    }//end of y
    count++;
    
   // pnm_freepamarray(previous,&temppam);
  }//end of while
  
  

  return newColorProfile;
}





/**
 * Takes a color image and returns a PAM (and correspondingly allocated
 * tuple image) of it in greyscale (arithmetic averaged) form.
 * 
 * Returns the image tuple, modifies the new pam by reference.
 * 
 * Since PAM uses cstrs, we need to use strcmp and strcpy.
 */
tuple** ColorToGrey(tuple** inputImage, const pam& inpam, pam& greyPam)
{
  greyPam = inpam;
  strcpy(greyPam.tuple_type, "GRAYSCALE");
  greyPam.format = 20533;	//greyscale - color is 20534.
  greyPam.depth = 1;
  tuple** greyArray = pnm_allocpamarray(&greyPam);
  
  if(DEBUG == TRUE)
  {
    cerr << "Converting " << inpam.width << "x" << inpam.height << " image to GRAYSCALE." << endl;
  }
  
  if(strcmp("GRAYSCALE", inpam.tuple_type) == 0)
  {
    if(DEBUG)
      cout << "Source image is already a Grayscale image.";
    return greyArray;
  }
  else if(strcmp("RGB", inpam.tuple_type) == 0)
  {
    for(int y = 0; y < inpam.height; y++)
    {
      for(int x = 0; x < inpam.width; x++)
      {
	greyArray[y][x][0] = (inputImage[y][x][0] + inputImage[y][x][1] + inputImage[y][x][2]) / 3;
      }
    }
  }
  
  return greyArray;
}


