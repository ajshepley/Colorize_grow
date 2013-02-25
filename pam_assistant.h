#ifndef PAM_ASSISTANT_H
#define PAM_ASSISTANT_H

extern "C"
{
  #include <pam.h>
}

/* The latest compatible Ubuntu release of NetPBM doesn't properly define
 * these macros, so for the sake of backwards compatibility, we redefine
 * them here if needed. */

#ifndef PNM_EQUAL
#define PNM_EQUAL(x,y) PPM_EQUAL(x,y)
#endif

#ifndef PAM_MEMBER_OFFSET
#define PAM_MEMBER_OFFSET(mbrname) \
  ((size_t)(unsigned long)(char*)&((struct pam *)0)->mbrname)
#endif

#ifndef PAM_MEMBER_SIZE
#define PAM_MEMBER_SIZE(mbrname) \
  sizeof(((struct pam *)0)->mbrname)
#endif

#ifndef PAM_STRUCT_SIZE
#define PAM_STRUCT_SIZE(mbrname) \
  (PAM_MEMBER_OFFSET(mbrname) + PAM_MEMBER_SIZE(mbrname))
#endif

using namespace std;


tuple **read_image(char *filename, pam &inpam);

void write_image(char *filename, const pam &newpam, tuple **array);

#endif