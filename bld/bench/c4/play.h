void BACKMOV()
{
  u_int mask,d;
  int hi,h,n,v;

  n = moves[--plycnt];
  if ((hi = height[n] += 8) < 24)
    l[r[n]]=r[l[n]]=n;
  h = hi>>3;
  columns[n] >>= 1;
  mask = ~(0x80 >> n);
  ROWS[h] = (d = ROWS[h]) & mask;
  if (v = thrcols[d]) {
    --THRCNT[hi+((v&7)-n)];
    if (v >>= 3)
      --THRCNT[hi+(v-n)];
  }
  if (d = DIAS[6+n+h]) {
    DIAS[6+n+h] = d & mask;
    if (v = thrcols[d]) {
      --THRCNT[hi-7*((v&7)-n)];
      if (v >>= 3)
        --THRCNT[hi-7*(v-n)];
    }
  }
  if (d = DIAS[6+n-h]) {
    DIAS[6+n-h] = d & mask;
    if (v = thrcols[d]) {
      --THRCNT[hi+9*((v&7)-n)];
      if (v >>= 3)
        --THRCNT[hi+9*(v-n)];
    }
  }
}

void MAKEMOV(n) 
int n;
{
  u_int mask,d;
  int hi,h,v;

  moves[plycnt++] = n;
  hi = height[n];
  h = hi>>3;
  if ((height[n] = hi-8) < 16)
    l[r[l[n]]=r[n]]=l[n];
  columns[n] = columns[n] << 1 BIT;
  mask = 0x80 >> n;
  d = ROWS[h] |= mask;
  if (v = thrcols[d]) {
    ++THRCNT[hi+((v&7)-n)];
    if (v >>= 3)
      ++THRCNT[hi+(v-n)];
  }
  if (d = DIAS[6+n+h]) {
    DIAS[6+n+h] = d |= mask;
    if (v = thrcols[d]) {
      ++THRCNT[hi-7*((v&7)-n)];
      if (v >>= 3)
        ++THRCNT[hi-7*(v-n)];
    }
  }
  if (d = DIAS[6+n-h]) {
    DIAS[6+n-h] = d |= mask;
    if (v = thrcols[d]) {
      ++THRCNT[hi+9*((v&7)-n)];
      if (v >>= 3)
        ++THRCNT[hi+9*(v-n)];
    }
  }
}

#undef BACKMOV
#undef MAKEMOV
#undef ROWS
#undef DIAS
#undef THRCNT
#undef BIT
