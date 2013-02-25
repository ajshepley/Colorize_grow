#include "pam_assistant.h"

#include <iostream>
using namespace std;

/* reads the image into the netpbm structure */
tuple **read_image(char *filename, pam &inpam)
{
  FILE *f;
  tuple **A;

  if ((f = pm_openr(filename)) == NULL) {
    cerr << "Cannot open file \"" << filename << "\" for reading." << endl;
    exit(1);
  }

  if ((A = pnm_readpam(f, &inpam, PAM_STRUCT_SIZE(tuple_type))) == NULL) {
    cerr << "Cannot read image \"" << filename << "\"." << endl;
    exit(1);
  }
  
  pm_close(f);
  return A;
}

/* writes the image to the given file 
 * Now adjusts for image size and attribute changes. */
void write_image(char *filename, const pam &newpam, tuple **array)
{
  FILE *f;
  pam outpam = newpam;
  
  if ((f = pm_openw(filename)) == NULL) {
    cerr << "Cannot open file \"" << filename << "\" for writing." << endl;
    exit(1);
  }
  else
    cout << "Writing Image to file..." << endl;

  /* NOTE: if you change other attributes such as height and width, you
     should change it here too. */
  outpam.file = f;
  
  pnm_writepam(&outpam, array);

  pm_close(f);
  cout << "Complete." << endl;
}

