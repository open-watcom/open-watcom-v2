// diagnose CV effects with member ptrs
//
//

#define CV
#define CVc const
#define CVv volatile
#define CVcv CVc CVv

#define MP_   int CV   S::*
#define MP_C  int CVc  S::*
#define MP_V  int CVv  S::*
#define MP_CV int CVcv S::*

struct S {
    int i;
};


// should be ok: (can add qualification)

MP_   mp01 = (MP_  )&S::i;
MP_C  mp02 = (MP_  )&S::i;
MP_V  mp03 = (MP_  )&S::i;
MP_CV mp04 = (MP_  )&S::i;
MP_C  mp05 = (MP_C )&S::i;
MP_CV mp06 = (MP_C )&S::i;
MP_V  mp07 = (MP_V )&S::i;
MP_CV mp08 = (MP_V )&S::i;
MP_CV mp09 = (MP_CV)&S::i;

// should be err: (can't subtract qualification)

MP_   mp10 = (MP_C )&S::i;
MP_V  mp11 = (MP_C )&S::i;
MP_   mp12 = (MP_V )&S::i;
MP_C  mp13 = (MP_V )&S::i;
MP_   mp14 = (MP_CV)&S::i;
MP_C  mp15 = (MP_CV)&S::i;
MP_V  mp16 = (MP_CV)&S::i;

// should be ok: (one can be implicitly converted to other)

bool v01 = mp01 == (MP_  )&S::i;
bool v02 = mp02 == (MP_  )&S::i;
bool v03 = mp03 == (MP_  )&S::i;
bool v04 = mp04 == (MP_  )&S::i;
bool v05 = mp05 == (MP_C )&S::i;
bool v06 = mp06 == (MP_C )&S::i;
bool v07 = mp07 == (MP_V )&S::i;
bool v08 = mp08 == (MP_V )&S::i;
bool v09 = mp09 == (MP_CV)&S::i;
bool v10 = mp10 == (MP_C )&S::i;
bool v12 = mp12 == (MP_V )&S::i;
bool v14 = mp14 == (MP_CV)&S::i;
bool v15 = mp15 == (MP_CV)&S::i;
bool v16 = mp16 == (MP_CV)&S::i;

// should be err: (can't subtract qualification)

bool v11 = mp11 == (MP_C )&S::i;
bool v13 = mp13 == (MP_V )&S::i;


