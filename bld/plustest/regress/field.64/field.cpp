#pragma inline_depth(0);
/* @(#)alloc.C  1.3   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>               // Needed for all math functions

#include "field.h"
#include "fldalloc.h"

#define NUM_BUCKETS 8
#define NUM_BUCKETS_M1 (NUM_BUCKETS-1)
#define HASH_SIZE_OFFSET 3

static int NMC_OFFSET = -1;     /* Must be initialized at runtime */

typedef struct BucketST Bucket;

typedef struct MemoryChainST {
  struct MemoryChainST* next;
  struct MemoryChainST* next_free;
  Bucket *my_bucket;
  double ptr[1];
} MemoryChain;

static struct BucketST {
  size_t size;
  MemoryChain* root;
  MemoryChain* next_free;
  Bucket* next;
} *size_bucket[NUM_BUCKETS];

#define HashSizeCode(val) (NUM_BUCKETS_M1 & ((size_t)(val) >> HASH_SIZE_OFFSET))

void* __field_alloc_Allocate(size_t size){
  Bucket *new_entry;
  size_t n = HashSizeCode(size);
  Bucket *entry = size_bucket[n];
  while (entry){
    if (entry->size == size){
      MemoryChain *ptr = entry->next_free;
      if (ptr){
        entry->next_free = ptr->next_free;
        return (void*)ptr->ptr;
      }else{
        MemoryChain *new_chunk = 
            (MemoryChain*)malloc(size + sizeof(MemoryChain));
        if (!new_chunk) return 0;
        new_chunk->next = entry->root;
        new_chunk->next_free = NULL;
        new_chunk->my_bucket = entry;
        entry->root = new_chunk;
        entry->next_free = new_chunk;
        return __field_alloc_Allocate(size);
      }
    }
    entry = entry->next;
  }
  new_entry = (Bucket*)malloc(sizeof(Bucket));
  if (!new_entry) return 0;
  new_entry->size = size;
  new_entry->root = NULL;
  new_entry->next_free = NULL;
  new_entry->next = size_bucket[n];
  size_bucket[n] = new_entry;
  return __field_alloc_Allocate(size);
}

void __field_alloc_Deallocate(void* returned_pointer){
  MemoryChain *rtn;
  if (NMC_OFFSET == -1){
    NMC_OFFSET = (char*)((MemoryChain*)0)->ptr - (char*)(MemoryChain*)0;
  }
  rtn = (MemoryChain*)((char*)returned_pointer - NMC_OFFSET);
  rtn->next_free = rtn->my_bucket->next_free;
  rtn->my_bucket->next_free = rtn;
}

void __field_alloc_collect_garbage(void){
  /* nothing implemented just now */
  fprintf(stderr, "Out of memory; aborting ...\n"); 
  abort();
}
// @(#)f_acos.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field acos(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = acos(f.root->data[i]);
  return rtn;
}
// @(#)f_asin.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field asin(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = asin(f.root->data[i]);
  return rtn;
}
// @(#)f_assem_1.C      1.1   Release Date: 2/19/93


field& field::Assemble(const field& b, const ifield& ia, const ifield& ib){
  const size_t l = ia.Length();
  assert(l == ib.Length());
  Private();
  field& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] += b[ib[i]];
  return This;
}

// @(#)f_assem_2.C      1.1   Release Date: 2/19/93


field& field::Assemble(const field& b, const ifield& ia){
  const size_t l = ia.Length();
  assert(l == b.Length());
  Private();
  field& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] += b[i];
  return This;
}

// @(#)f_assem_3.C      1.1   Release Date: 2/19/93


field& field::Assemble(double b, const ifield& ia){
  const size_t l = ia.Length();
  Private();
  field& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] += b;
  return This;
}

// @(#)f_atan.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field atan(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = atan(f.root->data[i]);
  return rtn;
}
// @(#)f_atan2_df.C     1.2   Release Date: 2/19/93



field atan2(double a, const field& b){
  size_t l = b.Length();
  field rtn(l);
  double *rp = rtn;
  const double *bp = b;
  for (register i=0; i<l; i++) rp[i] = atan2(a, bp[i]);
  return rtn;
}
// @(#)f_atan2_fd.C     1.2   Release Date: 2/19/93



field atan2(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  double *rp = rtn;
  const double *ap = a;
  for (register i=0; i<l; i++) rp[i] = atan2(ap[i], b);
  return rtn;
}
// @(#)f_atan2_ff.C     1.1   Release Date: 2/19/93



field atan2(const field& a, const field& b){
  size_t l = a.root->length;
  assert(b.root->length == l);
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = atan2(a.root->data[i], b.root->data[i]);
  return rtn;
}
// @(#)f_ceil.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field ceil(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = ceil(f.root->data[i]);
  return rtn;
}
// @(#)f_cos.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field cos(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = cos(f.root->data[i]);
  return rtn;
}
// @(#)f_cosh.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field cosh(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = cosh(f.root->data[i]);
  return rtn;
}
// @(#)f_ct_a.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field::field(size_t len_a, const double *a) :
  ref_flag(0),
  root(new(len_a) __field_node(len_a))
{
  if (a) memcpy(root->data, a, sizeof(double)*len_a);
}
// @(#)f_ct_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field::field(const field& f) :
  ref_flag(0)
{
  if (!f.root){
    root = NULL;
  }else if (f.ref_flag){
    root = f.root->NewCopy();
  }else{
    root = f.root;
    f.root->ref_count++;
  }
}
// @(#)f_di_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator/=(double d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] /= d;
  return *this;
}
// @(#)f_di_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator/(double d, const field& a){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d / a.root->data[i];
  return rtn;
}
// @(#)f_di_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator/=(const field& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] /= f.root->data[i];
  return *this;
}
// @(#)f_di_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator/(const field& a, double d){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] / d;
  return rtn;
}
// @(#)f_di_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator/(const field& a, const field& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] / b.root->data[i];
  return rtn;
}
// @(#)f_dim_df.C       1.2   Release Date: 2/19/93



field dim(double a, const field& b){
  size_t l = b.Length();
  field rtn(l);
  double *rp = rtn;
  const double *bp = b;
  for (register i=0; i<l; i++) rp[i] = (a > bp[i]? a - bp[i] : 0.0);
  return rtn;
}
// @(#)f_dim_fd.C       1.1   Release Date: 2/19/93



field dim(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b? a.root->data[i] - b : 0.0);
  return rtn;
}
// @(#)f_dim_ff.C       1.2   Release Date: 2/19/93



field dim(const field& a, const field& b){
  size_t l = b.Length();
  assert(a.Length() == l);
  field rtn(l);
  double *rp = rtn;
  const double *ap=a, *bp=b;
  for (register i=0; i<l; i++) rp[i] = (ap[i] > bp[i]? ap[i] - bp[i] : 0.0);
  return rtn;
}
// @(#)f_eq.C   1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator=(const field& f){
  if (!f.root){
    Free();
    root = NULL;
  }else if (this == &f){
    return *this;
  }else if (f.ref_flag){
    Free();
    root = f.root->NewCopy();
  }else{
    Free();
    root = f.root;
    f.root->ref_count++;
  }
  ref_flag = 0;
  return *this;
}
// @(#)f_eq_d.C 1.1   Release Date: 2/26/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator=(double fill){
  if (root){
    if (root->ref_count > 1) Private();
    ref_flag = 0;
    const size_t l = root->length;
    for (register i=0; i<l; i++) root->data[i] = fill;
  }
  return *this;
}
// @(#)f_eq_i.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator=(const ifield& f){
  if (!f.root){
    Free();
    root = NULL;
  }else{
    Free();
    root = new(f.root->length) __field_node(f.root->length);
    for (register i=0; i<root->length; i++) 
      root->data[i] = (double)f.root->data[i];
  }
  ref_flag = 0;
  return *this;
}
// @(#)f_exp.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field exp(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = exp(f.root->data[i]);
  return rtn;
}
// @(#)f_fabs.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field fabs(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = fabs(f.root->data[i]);
  return rtn;
}
// @(#)f_floor.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field floor(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = floor(f.root->data[i]);
  return rtn;
}
// @(#)f_fmod_df.C      1.2   Release Date: 2/19/93



field fmod(double a, const field& b){
  size_t l = b.Length();
  field rtn(l);
  double *rp = rtn;
  const double *bp = b;
  for (register i=0; i<l; i++) rp[i] = fmod(a, bp[i]);
  return rtn;
}
// @(#)f_fmod_fd.C      1.2   Release Date: 2/19/93



field fmod(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  double *rp = rtn;
  const double *ap = a;
  for (register i=0; i<l; i++) rp[i] = fmod(ap[i], b);
  return rtn;
}
// @(#)f_fmod_ff.C      1.1   Release Date: 2/19/93



field fmod(const field& a, const field& b){
  size_t l = a.root->length;
  assert(b.root->length == l);
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = fmod(a.root->data[i], b.root->data[i]);
  return rtn;
}
// @(#)f_frexp.C        1.2   Release Date: 2/19/93



field frexp(const field& a, ifield& b){
  size_t l = a.Length();
  field rtn(l);
  b = ifield(l);
  double *rp = rtn;
  const double *ap = a;
  INT *bp = b;
  for (register i=0; i<l; i++) {
      int d;
      rp[i] = frexp(ap[i], &d);
      bp[i] = d;
  }
  return rtn;
}
// @(#)f_ftoi.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field::field(const ifield& f) :
  ref_flag(0)
{
  if (!f.root){
    root = NULL;
  }else{
    root = new(f.root->length) __field_node(f.root->length);
    for (register i=0; i<root->length; i++)
      root->data[i] = (int)f.root->data[i];
  }
}
// @(#)f_gather_1.C     1.1   Release Date: 2/19/93


field field::Gather(const ifield& ia) const {
  size_t l = ia.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = root->data[ia[i]];
  return rtn;
}
// @(#)f_gather_2.C     1.2   Release Date: 2/19/93


field field::Gather(size_t start, size_t end, size_t stride) const {
  assert(end < root->length);
  field rtn((end-start)/stride + 1);
  double *rp = rtn;
  const double* This = *this;
  for (register i=start, j=0; i<end; i+=stride,j++) rp[j] = This[i];
  return rtn;
}
// @(#)f_ge_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>=(double b, const field& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b >= a.root->data[i]);
  return rtn;
}
// @(#)f_ge_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>=(const field& a, double b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] >= b);
  return rtn;
}
// @(#)f_ge_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>=(const field& a, const field& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] >= b.root->data[i]);
  return rtn;
}
// @(#)f_gt_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>(double b, const field& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b > a.root->data[i]);
  return rtn;
}
// @(#)f_gt_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>(const field& a, double b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] > b);
  return rtn;
}
// @(#)f_gt_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>(const field& a, const field& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] > b.root->data[i]);
  return rtn;
}
// @(#)f_io.C   1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


// I/O

size_t fread(field& f, FILE* d){
  int length;
  size_t rtn;
  f.Free();
  rtn = fread((char*)&length, sizeof(size_t), 1, d);
  f.root = new(length) __field_node(length);
  rtn += fread((char*)f.root->data, sizeof(double), length, d);
  f.ref_flag = 0;
  return (rtn == f.root->length+1? 1 : 0);
}

size_t fwrite(const field& f, FILE* d){
  size_t rtn;
  if (f.Length() != 0){
    rtn = fwrite((char*)&f.root->length, sizeof(size_t), 1, d);
    rtn += fwrite((char*)&(f.root->data[0]), sizeof(double), f.root->length, d);
    return (rtn == f.root->length+1? 1 : 0);
  }else{
    const size_t ZERO = 0;
    rtn = fwrite((char*)&ZERO, sizeof(size_t), 1, d);
    return (rtn == 1? 1 : 0);
  }
}
// @(#)f_ldexp.C        1.2   Release Date: 2/19/93



field ldexp(const field& a, const ifield& b){
  size_t l = a.Length();
  assert(l == b.Length());
  field rtn(l);
  double *rp = rtn;
  const double *ap = a;
  const INT *bp = b;
  for (register i=0; i<l; i++) rp[i] = ldexp(ap[i], bp[i]);
  return rtn;
}
// @(#)f_le_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<=(double b, const field& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b <= a.root->data[i]);
  return rtn;
}
// @(#)f_le_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<=(const field& a, double b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] <= b);
  return rtn;
}
// @(#)f_le_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<=(const field& a, const field& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] <= b.root->data[i]);
  return rtn;
}
// @(#)f_leq_df.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator==(double b, const field& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b == a.root->data[i]);
  return rtn;
}
// @(#)f_leq_fd.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator==(const field& a, double b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] == b);
  return rtn;
}
// @(#)f_leq_ff.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator==(const field& a, const field& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] == b.root->data[i]);
  return rtn;
}
// @(#)f_lne_df.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator!=(double b, const field& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b != a.root->data[i]);
  return rtn;
}
// @(#)f_lne_fd.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator!=(const field& a, double b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] != b);
  return rtn;
}
// @(#)f_lne_ff.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator!=(const field& a, const field& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] != b.root->data[i]);
  return rtn;
}
// @(#)f_log.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field log(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = log(f.root->data[i]);
  return rtn;
}
// @(#)f_log10.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field log10(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = log10(f.root->data[i]);
  return rtn;
}
// @(#)f_lt_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<(double b, const field& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b < a.root->data[i]);
  return rtn;
}
// @(#)f_lt_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<(const field& a, double b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] < b);
  return rtn;
}
// @(#)f_lt_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<(const field& a, const field& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] < b.root->data[i]);
  return rtn;
}
// @(#)f_max_df.C       1.1   Release Date: 2/19/93



field max(double b, const field& a){
  size_t l = a.Length();
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b? a.root->data[i] : b);
  return rtn;
}
// @(#)f_max_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


double max(const field& f){
  size_t l = f.root->length;
  double retval = f.root->data[l-1];
  for (register i=0; i<l-1; i++) 
    retval = (retval > f.root->data[i]? retval : f.root->data[i]);
  return retval;
} 
// @(#)f_max_fd.C       1.1   Release Date: 2/19/93



field max(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b? a.root->data[i] : b);
  return rtn;
}
// @(#)f_max_ff.C       1.1   Release Date: 2/19/93



field max(const field& a, const field& b){
  size_t l = a.Length();
  assert(l == b.root->length);
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b.root->data[i] ?
                                a.root->data[i] : b.root->data[i]);
  return rtn;
}
// @(#)f_mi_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator-=(double d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] -= d;
  return *this;
}
// @(#)f_mi_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator-(double d, const field& a){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d - a.root->data[i];
  return rtn;
}
// @(#)f_mi_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator-=(const field& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] -= f.root->data[i];
  return *this;
}
// @(#)f_mi_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator-(const field& a, double d){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] - d;
  return rtn;
}
// @(#)f_mi_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator-(const field& a, const field& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] - b.root->data[i];
  return rtn;
}
// @(#)f_min_df.C       1.1   Release Date: 2/19/93



field min(double b, const field& a){
  size_t l = a.Length();
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < b? a.root->data[i] : b);
  return rtn;
}
// @(#)f_min_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


double min(const field& f){
  size_t l = f.root->length;
  double retval = f.root->data[l-1];
  for (register i=0; i<l-1; i++) 
    retval = (retval < f.root->data[i]? retval : f.root->data[i]);
  return retval;
} 
// @(#)f_min_fd.C       1.1   Release Date: 2/19/93



field min(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < b? a.root->data[i] : b);
  return rtn;
}
// @(#)f_min_ff.C       1.1   Release Date: 2/19/93



field min(const field& a, const field& b){
  size_t l = a.Length();
  assert(l == b.root->length);
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < b.root->data[i] ?
                                a.root->data[i] : b.root->data[i]);
  return rtn;
}
// @(#)f_modf.C 1.2   Release Date: 2/19/93



field modf(const field& a, field& b){
  size_t l = a.Length();
  field rtn(l);
  b = field(l);
  double *rp = rtn, *bp = b;
  const double *ap = a;
  for (register i=0; i<l; i++) rp[i] = modf(ap[i], bp + i);
  return rtn;
}
// @(#)f_mu_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator*=(double d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] *= d;
  return *this;
}
// @(#)f_mu_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator*(double d, const field& a){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d * a.root->data[i];
  return rtn;
}
// @(#)f_mu_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator*=(const field& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] *= f.root->data[i];
  return *this;
}
// @(#)f_mu_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator*(const field& a, double d){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] * d;
  return rtn;
}
// @(#)f_mu_ff.C        1.2   Release Date: 3/1/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator*(const field& a, const field& b){
  assert(a.root->length == b.root->length);
  const size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] * b.root->data[i];
  return rtn;
}
// @(#)f_pf.C   1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


void field::Free(void) {
  if (root && !(--root->ref_count)){
    delete root;
  }
  root = 0;
}

void field::Private(void) {
  if (root && root->ref_count > 1){
    __field_node* tmp = root->NewCopy();
    Free();
    root = tmp;
  }
}
// @(#)f_pl_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator+=(double d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] += d;
  return *this;
}
// @(#)f_pl_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator+(double d, const field& a){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d + a.root->data[i];
  return rtn;
}
// @(#)f_pl_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field& field::operator+=(const field& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] += f.root->data[i];
  return *this;
}
// @(#)f_pl_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator+(const field& a, double d){
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] + d;
  return rtn;
}
// @(#)f_pl_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field operator+(const field& a, const field& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] + b.root->data[i];
  return rtn;
}
// @(#)f_pow_df.C       1.1   Release Date: 2/19/93



field pow(double a, const field& b){
  size_t l = b.Length();
  field rtn(l);
  double *rp = rtn;
  const double *bp = b;
  for (register i=0; i<l; i++) rp[i] = pow(a, bp[i]);
  return rtn;
}
// @(#)f_pow_fd.C       1.1   Release Date: 2/19/93



field pow(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  double *rp = rtn;
  const double *ap = a;
  for (register i=0; i<l; i++) rp[i] = pow(ap[i], b);
  return rtn;
}
// @(#)f_pow_ff.C       1.1   Release Date: 2/19/93



field pow(const field& a, const field& b){
  size_t l = a.root->length;
  assert(b.root->length == l);
  field rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = pow(a.root->data[i], b.root->data[i]);
  return rtn;
}
// @(#)f_scatter_1.C    1.2   Release Date: 2/19/93


field& field::Scatter(const field& b, const ifield& ia, const ifield& ib){
  size_t l = ia.Length();
  assert (l = ib.Length());
  double *This = *this;
  const double *bp = b;
  const INT *iap = ia, *ibp = ib;
  for (register i=0; i<l; i++)  This[iap[i]] = bp[ibp[i]];
  return *this;
}
// @(#)f_scatter_2.C    1.1   Release Date: 2/19/93



field& field::Scatter(const field& b, const ifield& ia){
  const size_t l = ia.Length();
  assert(l == b.Length());
  field& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] = b[i];
  return This;
}

// @(#)f_scatter_3.C    1.1   Release Date: 2/19/93



field& field::Scatter(double b, const ifield& ia){
  const size_t l = ia.Length();
  field& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] = b;
  return This;
}

// @(#)f_scatter_4.C    1.2   Release Date: 2/19/93


field& field::Scatter(const field& a, size_t start, size_t end, size_t stride){
  assert(end < root->length);
  assert((end-start)/stride < a.root->length);
  double *This = *this;
  const double *ap = a;
  for (register i=start, j=0; i<end; i+=stride,j++)  This[i] = ap[j];
  return *this;
}
// @(#)f_shift.C        1.2   Release Date: 2/26/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field field::Shift(int i) const{
  size_t l = root->length;
  field rtn(l);
  if (i>=0) memcpy(rtn.root->data+1, root->data, (l-i)*sizeof(double));
       else memcpy(rtn.root->data, root->data-i, (l+i)*sizeof(double));
  return rtn;
}
// @(#)f_sign_df.C      1.1   Release Date: 2/19/93



field sign(double b, const field& a){
  size_t l = a.Length();
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < 0.0? -b : b);
  return rtn;
}
// @(#)f_sign_fd.C      1.2   Release Date: 2/19/93



field sign(const field& a, double b){
  size_t l = a.Length();
  field rtn(l);
  double *rp = rtn;
  const double *ap = a;
  for (register i=0; i<l; i++) rp[i] = (b<0.0? -ap[i] : ap[i]);
  return rtn;
}
// @(#)f_sign_ff.C      1.1   Release Date: 2/19/93



field sign(const field& a, const field& b){
  size_t l = a.Length();
  assert(l == b.root->length);
  field rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (b.root->data[i] < 0.0? -a.root->data[i] :
                                                 a.root->data[i]);
  return rtn;
}
// @(#)f_sin.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field sin(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = sin(f.root->data[i]);
  return rtn;
}
// @(#)f_sinh.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field sinh(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = sinh(f.root->data[i]);
  return rtn;
}
// @(#)f_sqrt.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field sqrt(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = sqrt(f.root->data[i]);
  return rtn;
}
// @(#)f_sum.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


double field::Sum(void) const{
  double retval = 0.0;
  size_t l = root->length;
  for (register i=0; i<l; i++) retval += root->data[i];
  return retval;
}
// @(#)f_tan.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field tan(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = tan(f.root->data[i]);
  return rtn;
}
// @(#)f_tanh.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



field tanh(const field& f){
  size_t l = f.root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = tanh(f.root->data[i]);
  return rtn;
}
// @(#)f_umi.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


field field::operator-(void) const{
  size_t l = root->length;
  field rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = -root->data[i];
  return rtn;
}
// @(#)fn_ct_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



__field_node::__field_node(const __field_node& src) :
  length(src.length),
  ref_count(1)
{
  memcpy(data, src.data, sizeof(double)*length);
}
// @(#)fn_ct_ud.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


__field_node::__field_node(size_t size, double fill) :
  length(size),
  ref_count(1)
{
  for (register i=0; i<size; i++) data[i] = fill;
}
// @(#)fn_eq.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



__field_node& __field_node::operator=(const __field_node& src){
  length = src.length;
  ref_count = 1;
  memcpy(data, src.data, sizeof(double)*length);
  return *this;
}
// @(#)fn_new.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


void* __field_node::operator new(size_t count, size_t length){
  assert(count == sizeof(__field_node));

RETRY:
  void* ptr = __field_alloc_Allocate(length*sizeof(double) + 
                                     sizeof(__field_node));

  if (!ptr){
    __field_alloc_collect_garbage();
    goto RETRY;
  }
  return ptr;
}
void* __field_node::operator new(size_t count){
    return __field_node::operator new( count, 1 );
}

void __field_node::operator delete(void* ptr){
  __field_alloc_Deallocate(ptr);
}
// @(#)i_abs.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



ifield abs(const ifield& f){
  size_t l = f.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (f.root->data[i] < 0?  -f.root->data[i] :
                                                f.root->data[i]);
  return rtn;
}
// @(#)i_assem_1.C      1.2   Release Date: 2/19/93


ifield& ifield::Assemble(const ifield& b, const ifield& ia, const ifield& ib){
  const size_t l = ia.Length();
  assert(l == ib.Length());
  Private();
  ifield& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] += b[ib[i]];
  return This;
}

// @(#)i_assem_2.C      1.2   Release Date: 2/19/93


ifield& ifield::Assemble(const ifield& b, const ifield& ia){
  const size_t l = ia.Length();
  assert(l == b.Length());
  Private();
  ifield& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] += b[i];
  return This;
}

// @(#)i_assem_3.C      1.2   Release Date: 2/19/93


ifield& ifield::Assemble(INT b, const ifield& ia){
  const size_t l = ia.Length();
  Private();
  ifield& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] += b;
  return This;
}

// @(#)i_band_d.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator&=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] &= d;
  return *this;
}
// @(#)i_band_df.C      1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator&(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (d & a.root->data[i]);
  return rtn;
}
// @(#)i_band_f.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator&=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] &= f.root->data[i];
  return *this;
}
// @(#)i_band_fd.C      1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator&(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] & d);
  return rtn;
}
// @(#)i_band_ff.C      1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator&(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] & b.root->data[i]);
  return rtn;
}
// @(#)i_bor_d.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator|=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] |= d;
  return *this;
}
// @(#)i_bor_df.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator|(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (d | a.root->data[i]);
  return rtn;
}
// @(#)i_bor_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator|=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] |= f.root->data[i];
  return *this;
}
// @(#)i_bor_fd.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator|(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] | d);
  return rtn;
}
// @(#)i_bor_ff.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator|(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] | b.root->data[i]);
  return rtn;
}
// @(#)i_com.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield ifield::operator~(void) const{
  size_t l = root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = ~root->data[i];
  return rtn;
}
// @(#)i_ct_a.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



ifield::ifield(size_t len_a, const INT *a) :
  ref_flag(0),
  root(new(len_a) __field_inode(len_a))
{
  memcpy(root->data, a, sizeof(*a)*len_a);
}
// @(#)i_ct_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield::ifield(const ifield& f) :
  ref_flag(0)
{
  if (!f.root){
    root = NULL;
  }else if (f.ref_flag){
    root = f.root->NewCopy();
  }else{
    root = f.root;
    f.root->ref_count++;
  }
}
// @(#)i_di_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator/=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] /= d;
  return *this;
}
// @(#)i_di_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator/(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d / a.root->data[i];
  return rtn;
}
// @(#)i_di_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator/=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] /= f.root->data[i];
  return *this;
}
// @(#)i_di_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator/(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] / d;
  return rtn;
}
// @(#)i_di_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator/(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] / b.root->data[i];
  return rtn;
}
// @(#)i_dim_df.C       1.1   Release Date: 2/19/93


ifield dim(INT a, const ifield& b){
  const size_t l = b.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)  rtn[i] = (a > b[i]? a - b[i] : 0);
  return rtn;
}

// @(#)i_dim_fd.C       1.1   Release Date: 2/19/93


ifield dim(const ifield& a, INT b){
  const size_t l = a.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)  rtn[i] = (a[i] > b? a[i] - b : 0);
  return rtn;
}

// @(#)i_dim_ff.C       1.2   Release Date: 2/19/93


ifield dim(const ifield& a, const ifield& b){
  const size_t l = a.Length();
  assert(l == b.Length());
  ifield rtn(l);
  for (register i=0; i<l; i++)  rtn[i] = (a[i] > b[i]? a[i] - b[i] : 0);
  return rtn;
}

// @(#)i_div_df.C       1.1   Release Date: 2/19/93



void div(int a, const ifield& b, ifield& c, ifield& d){
  const size_t l = b.Length();
  c = ifield(l);
  d = ifield(l);
  for (register i=0; i<l; i++){
    c[i] = a/b[i];
    d[i] = a%b[i];
  }
}

// @(#)i_div_fd.C       1.1   Release Date: 2/19/93



void div(const ifield& a, int b, ifield& c, ifield& d){
  const size_t l = a.Length();
  c = ifield(l);
  d = ifield(l);
  for (register i=0; i<l; i++){
    c[i] = a[i]/b;
    d[i] = a[i]%b;
  }
}

// @(#)i_div_ff.C       1.1   Release Date: 2/19/93



void div(const ifield& a, const ifield& b, ifield& c, ifield& d){
  const size_t l = a.Length();
  assert(l == b.Length());
  c = ifield(l);
  d = ifield(l);
  for (register i=0; i<l; i++){
    c[i] = a[i]/b[i];
    d[i] = a[i]%b[i];
  }
}

// @(#)i_eq.C   1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator=(const ifield& f){
  if (!f.root){
    Free();
    root = NULL;
  }else if (this == &f){
   return *this;
  }else if (f.ref_flag){
    Free();
    root = f.root->NewCopy();
  }else{
    Free();
    root = f.root;
    f.root->ref_count++;
  }
  ref_flag = 0;
  return *this;
}
// @(#)i_eq_d.C 1.1   Release Date: 2/26/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator=(INT fill){
  if (root){
    if (root->ref_count > 1) Private();
    ref_flag = 0;
    const size_t l = root->length;
    for (register i=0; i<l; i++) root->data[i] = fill;
  }
  return *this;
}
// @(#)i_eq_i.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator=(const field& f){
  if (!f.root){
    Free();
    root = NULL;
  }else{
    Free();
    root = new(f.root->length) __field_inode(f.root->length);
    for (register i=0; i<root->length; i++) 
      root->data[i] = (int)f.root->data[i];
  }
  ref_flag = 0;
  return *this;
}
// @(#)i_gather_1.C     1.1   Release Date: 2/19/93


ifield ifield::Gather(const ifield& ia) const {
  size_t l = ia.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = root->data[ia[i]];
  return rtn;
}
// @(#)i_gather_2.C     1.1   Release Date: 2/19/93


ifield ifield::Gather(size_t start, size_t end, size_t stride) const {
  assert(end < root->length);
  ifield rtn((end-start)/stride + 1);
  INT *rp = rtn;
  const INT* This = *this;
  for (register i=start, j=0; i<end; i+=stride,j++) rp[j] = This[i];
  return rtn;
}
// @(#)i_ge_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>=(INT b, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b >= a.root->data[i]);
  return rtn;
}
// @(#)i_ge_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>=(const ifield& a, INT b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] >= b);
  return rtn;
}
// @(#)i_ge_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>=(const ifield& a, const ifield& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] >= b.root->data[i]);
  return rtn;
}
// @(#)i_gt_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>(INT b, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b > a.root->data[i]);
  return rtn;
}
// @(#)i_gt_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>(const ifield& a, INT b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] > b);
  return rtn;
}
// @(#)i_gt_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>(const ifield& a, const ifield& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] > b.root->data[i]);
  return rtn;
}
// @(#)i_io.C   1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


// I/O

size_t fread(ifield& f, FILE* d){
  int length;
  size_t rtn;
  f.Free();
  rtn = fread((char*)&length, sizeof(size_t), 1, d);
  f.root = new(length) __field_inode(length);
  rtn += fread((char*)f.root->data, sizeof(INT), length, d);
  f.ref_flag = 0;
  return (rtn == f.root->length+1? 1 : 0);
}

size_t fwrite(const ifield& f, FILE* d){
  size_t rtn;
  if (f.Length() != 0){
    rtn = fwrite((char*)&f.root->length, sizeof(size_t), 1, d);
    rtn += fwrite((char*)&(f.root->data[0]), sizeof(INT), f.root->length, d);
    return (rtn == f.root->length+1? 1 : 0);
  }else{
    const size_t ZERO = 0;
    rtn = fwrite((char*)&ZERO, sizeof(size_t), 1, d);
    return (rtn == 1? 1 : 0);
  }
}
// @(#)i_itof.C 1.2   Release Date: 2/19/93



ifield::ifield(const field& a) :
  ref_flag(0)
{
  if (a.root){
    const size_t l = a.root->length;
    root = new(l) __field_inode(l);
    for (register i=0; i<l; i++) root->data[i] = (int)a.root->data[i];
  }else{
    root = NULL;
  }
}

// @(#)i_land_df.C      1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator&&(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] && d);
  return rtn;
}
// @(#)i_land_fd.C      1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator&&(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] && d);
  return rtn;
}
// @(#)i_land_ff.C      1.2   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator&&(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] && b.root->data[i]);
  return rtn;
}
// @(#)i_le_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<=(INT b, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b <= a.root->data[i]);
  return rtn;
}
// @(#)i_le_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<=(const ifield& a, INT b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] <= b);
  return rtn;
}
// @(#)i_le_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<=(const ifield& a, const ifield& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] <= b.root->data[i]);
  return rtn;
}
// @(#)i_leq_df.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator==(INT b, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b == a.root->data[i]);
  return rtn;
}
// @(#)i_leq_fd.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator==(const ifield& a, INT b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] == b);
  return rtn;
}
// @(#)i_leq_ff.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator==(const ifield& a, const ifield& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] == b.root->data[i]);
  return rtn;
}
// @(#)i_lne_df.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator!=(INT b, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b != a.root->data[i]);
  return rtn;
}
// @(#)i_lne_fd.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator!=(const ifield& a, INT b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] != b);
  return rtn;
}
// @(#)i_lne_ff.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator!=(const ifield& a, const ifield& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] != b.root->data[i]);
  return rtn;
}
// @(#)i_lor_df.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator||(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] || d);
  return rtn;
}
// @(#)i_lor_fd.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator||(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] || d);
  return rtn;
}
// @(#)i_lor_ff.C       1.2   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator||(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] || b.root->data[i]);
  return rtn;
}
// @(#)i_ls_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator<<=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] <<= d;
  return *this;
}
// @(#)i_ls_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<<(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (d << a.root->data[i]);
  return rtn;
}
// @(#)i_ls_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator<<=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] <<= f.root->data[i];
  return *this;
}
// @(#)i_ls_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<<(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] << d;
  return rtn;
}
// @(#)i_ls_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<<(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] << b.root->data[i];
  return rtn;
}
// @(#)i_lt_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<(INT b, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (b < a.root->data[i]);
  return rtn;
}
// @(#)i_lt_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<(const ifield& a, INT b){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] < b);
  return rtn;
}
// @(#)i_lt_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator<(const ifield& a, const ifield& b){
  size_t l = a.root->length;
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] < b.root->data[i]);
  return rtn;
}
// @(#)i_max_df.C       1.1   Release Date: 2/19/93



ifield max(INT b, const ifield& a){
  size_t l = a.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b? a.root->data[i] : b);
  return rtn;
}
// @(#)i_max_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


INT max(const ifield& f){
  size_t l = f.root->length;
  int retval = f.root->data[l-1];
  for (register i=0; i<l-1; i++) 
    retval = (retval > f.root->data[i]? retval : f.root->data[i]);
  return retval;
} 
// @(#)i_max_fd.C       1.1   Release Date: 2/19/93



ifield max(const ifield& a, INT b){
  size_t l = a.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b? a.root->data[i] : b);
  return rtn;
}
// @(#)i_max_ff.C       1.1   Release Date: 2/19/93



ifield max(const ifield& a, const ifield& b){
  size_t l = a.Length();
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] > b.root->data[i] ?
                                a.root->data[i] : b.root->data[i]);
  return rtn;
}
// @(#)i_md_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator%=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] %= d;
  return *this;
}
// @(#)i_md_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator%(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d % a.root->data[i];
  return rtn;
}
// @(#)i_md_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator%=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] %= f.root->data[i];
  return *this;
}
// @(#)i_md_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator%(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] % d;
  return rtn;
}
// @(#)i_md_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator%(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] % b.root->data[i];
  return rtn;
}
// @(#)i_mi_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator-=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] -= d;
  return *this;
}
// @(#)i_mi_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator-(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d - a.root->data[i];
  return rtn;
}
// @(#)i_mi_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator-=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] -= f.root->data[i];
  return *this;
}
// @(#)i_mi_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator-(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] - d;
  return rtn;
}
// @(#)i_mi_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator-(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] - b.root->data[i];
  return rtn;
}
// @(#)i_min_df.C       1.1   Release Date: 2/19/93



ifield min(INT b, const ifield& a){
  size_t l = a.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < b? a.root->data[i] : b);
  return rtn;
}
// @(#)i_min_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


INT min(const ifield& f){
  size_t l = f.root->length;
  int retval = f.root->data[l-1];
  for (register i=0; i<l-1; i++) 
    retval = (retval < f.root->data[i]? retval : f.root->data[i]);
  return retval;
} 
// @(#)i_min_fd.C       1.1   Release Date: 2/19/93



ifield min(const ifield& a, INT b){
  size_t l = a.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < b? a.root->data[i] : b);
  return rtn;
}
// @(#)i_min_ff.C       1.1   Release Date: 2/19/93



ifield min(const ifield& a, const ifield& b){
  size_t l = a.Length();
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < b.root->data[i] ?
                                a.root->data[i] : b.root->data[i]);
  return rtn;
}
// @(#)i_mu_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator*=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] *= d;
  return *this;
}
// @(#)i_mu_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator*(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d * a.root->data[i];
  return rtn;
}
// @(#)i_mu_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator*=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] *= f.root->data[i];
  return *this;
}
// @(#)i_mu_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator*(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] * d;
  return rtn;
}
// @(#)i_mu_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator*(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] * b.root->data[i];
  return rtn;
}
// @(#)i_not.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield ifield::operator!(void) const{
  size_t l = root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = !root->data[i];
  return rtn;
}
// @(#)i_pf.C   1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


void ifield::Free(void) {
  if (root && !(--root->ref_count)){
    delete root;
  }
  root = 0;
}

void ifield::Private(void) {
  if (root && root->ref_count > 1){
    __field_inode* tmp = root->NewCopy();
    Free();
    root = tmp;
  }
}
// @(#)i_pl_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator+=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] += d;
  return *this;
}
// @(#)i_pl_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator+(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = d + a.root->data[i];
  return rtn;
}
// @(#)i_pl_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator+=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] += f.root->data[i];
  return *this;
}
// @(#)i_pl_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator+(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] + d;
  return rtn;
}
// @(#)i_pl_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator+(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] + b.root->data[i];
  return rtn;
}
// @(#)i_rand.C 1.3   Release Date: 2/19/93



void ifield::rand(void){
  Private();
  for (register i=0; i<root->length; i++)  root->data[i] = ::rand();
}
// @(#)i_rs_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator>>=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] >>= d;
  return *this;
}
// @(#)i_rs_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>>(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (d >> a.root->data[i]);
  return rtn;
}
// @(#)i_rs_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator>>=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] >>= f.root->data[i];
  return *this;
}
// @(#)i_rs_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>>(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = a.root->data[i] >> d;
  return rtn;
}
// @(#)i_rs_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator>>(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = a.root->data[i] >> b.root->data[i];
  return rtn;
}
// @(#)i_scatter_1.C    1.2   Release Date: 2/19/93



ifield& ifield::Scatter(const ifield& b, const ifield& ia, const ifield& ib){
  const size_t l = ia.Length();
  assert(l == ib.Length());
  ifield& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] = b[ib[i]];
  return This;
}

// @(#)i_scatter_2.C    1.1   Release Date: 2/19/93



ifield& ifield::Scatter(const ifield& b, const ifield& ia){
  const size_t l = ia.Length();
  assert(l == b.Length());
  ifield& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] = b[i];
  return This;
}

// @(#)i_scatter_3.C    1.1   Release Date: 2/19/93



ifield& ifield::Scatter(INT b, const ifield& ia){
  const size_t l = ia.Length();
  ifield& This = *this;
  for (register i=0; i<l; i++)  This[ia[i]] = b;
  return This;
}

// @(#)i_scatter_4.C    1.2   Release Date: 2/19/93


ifield& ifield::Scatter(const ifield& a, size_t start, size_t end,
                                 size_t stride){
  assert(end < root->length);
  assert((end-start)/stride < a.root->length);
  ifield& This = *this;
  for (register i=start, j=0; i<end; i+=stride,j++)
    This[i] = a[j];
  return This;
}
// @(#)i_shift.C        1.2   Release Date: 2/26/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



ifield ifield::Shift(int i) const{
  size_t l = root->length;
  ifield rtn(l);
  if (i>=0) memcpy(rtn.root->data+1, root->data, (l-i)*sizeof(INT));
       else memcpy(rtn.root->data, root->data-i, (l+i)*sizeof(INT));
  return rtn;
}
// @(#)i_sign_df.C      1.1   Release Date: 2/19/93



ifield sign(INT b, const ifield& a){
  size_t l = a.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (a.root->data[i] < 0.0? -b : b);
  return rtn;
}
// @(#)i_sign_fd.C      1.1   Release Date: 2/19/93



ifield sign(const ifield& ia, INT ib){
  const size_t l = ia.Length();
  ifield rtn(l);
  for (register i=0; i<l; i++)  rtn[i] = (ib<0? -ia[i] : ia[i]);
  return rtn;
}

// @(#)i_sign_ff.C      1.1   Release Date: 2/19/93



ifield sign(const ifield& a, const ifield& b){
  size_t l = a.Length();
  assert(l == b.root->length);
  ifield rtn(l);
  for (register i=0; i<l; i++)
    rtn.root->data[i] = (b.root->data[i] < 0.0? -a.root->data[i] :
                                                 a.root->data[i]);
  return rtn;
}
// @(#)i_sum.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


INT ifield::Sum(void) const{
  INT retval = 0;
  size_t l = root->length;
  for (register i=0; i<l; i++) retval += root->data[i];
  return retval;
}
// @(#)i_umi.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield ifield::operator-(void) const{
  size_t l = root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = -root->data[i];
  return rtn;
}
// @(#)i_xr_d.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator^=(INT d){
  assert(root!=NULL);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] ^= d;
  return *this;
}
// @(#)i_xr_df.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator^(INT d, const ifield& a){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (d ^ a.root->data[i]);
  return rtn;
}
// @(#)i_xr_f.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield& ifield::operator^=(const ifield& f){
  assert(root && f.root && root->length == f.root->length);
  Private();
  size_t l = root->length;
  for (register i=0; i<l; i++) root->data[i] ^= f.root->data[i];
  return *this;
}
// @(#)i_xr_fd.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator^(const ifield& a, INT d){
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) rtn.root->data[i] = (a.root->data[i] ^ d);
  return rtn;
}
// @(#)i_xr_ff.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


ifield operator^(const ifield& a, const ifield& b){
  assert(a.root->length == b.root->length);
  size_t l = a.root->length;
  ifield rtn(l);
  for (register i=0; i<l; i++) 
    rtn.root->data[i] = (a.root->data[i] ^ b.root->data[i]);
  return rtn;
}
// @(#)in_ct_f.C        1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



__field_inode::__field_inode(const __field_inode& src) :
  length(src.length),
  ref_count(1)
{
  memcpy(data, src.data, sizeof(INT)*length);
}
// @(#)in_ct_ui.C       1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


__field_inode::__field_inode(size_t size, INT fill) :
  length(size),
  ref_count(1)
{
  for (register i=0; i<size; i++)
    data[i] = fill;
}
// @(#)in_eq.C  1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 



__field_inode& __field_inode::operator=(const __field_inode& src){
  if (this == &src) return *this;
  length = src.length;
  ref_count = 1;
  memcpy(data, src.data, sizeof(INT)*length);
  return *this;
}
// @(#)in_new.C 1.1   Release Date: 2/19/93
//       Author:  Kent G. Budge, 
//                Computational Physics Research and Development (1431)
//                Sandia National Laboratories 


void* __field_inode::operator new(size_t count, size_t length){
  assert(count == sizeof(__field_inode));

RETRY:
  void* ptr = __field_alloc_Allocate(length*sizeof(INT) +
                                     sizeof(__field_inode));

  if (!ptr){
    __field_alloc_collect_garbage();
    goto RETRY;
  }
  return ptr;
}
void* __field_inode::operator new(size_t count){
    return __field_inode::operator new( count, 1 );
}

void __field_inode::operator delete(void* ptr){
  __field_alloc_Deallocate(ptr);
}
