#pragma once

#define SAFE_DELETE(a) { if(a) delete a; (a)=0; }
#define SAFE_DELETE_ARRAY(a) { if(a) delete [] a; (a)=0; }

inline unsigned int ilog2(register unsigned int x) 
{
    register unsigned int l=0;
    if(x >= 1<<16) { x>>=16; l|=16; }
    if(x >= 1<<8) { x>>=8; l|=8; }
    if(x >= 1<<4) { x>>=4; l|=4; }
    if(x >= 1<<2) { x>>=2; l|=2; }
    if(x >= 1<<1) l|=1;
    return l;
}

#define COMPILE_TIME_ASSERT(t) { char test[t? 1 : -1]; test[0] = 0; }

