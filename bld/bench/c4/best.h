int BOOKME()
{
  int score,bestscore,i,x,threat;

  nodes++;
  if (x = transpose(columns))
    return SCORE(x);

  if (plycnt >= 8) {
    for (threat=i=0; i=r[i]; ) {
      if (THRME[height[i]] || colthr[columns[i]]==ME)
        return IWIN;
      if (THROPP[height[i]] || colthr[columns[i]]==OPP)
        threat = threat << 3 | i;
    }
    if (threat > 7)
      return ILOSE;
    if (!threat) {
      score = bookin();
      transtore(columns, score, 31);
      return score;
    }
    MAKEMOV(threat);
    score = BOOKOPP();
    BACKMOV();
    return score;
  }
  for (bestscore=WORST,i=0; i=r[i]; ) {
    if (THRME[height[i]] || colthr[columns[i]]==ME) {
      bestscore = IWIN;
      continue;
    }
    MAKEMOV(i);
    if ((score = BOOKOPP()) IMPRVS bestscore)
      bestscore = score;
    BACKMOV();
  }
  transtore(columns, bestscore, 1);
  return bestscore;
}

int ABME(alpha, beta)
int alpha, beta;
{
  register int besti,i,j,k,l,val,score;
  int x,v,work;
  int rr[8];
  u_int poscnt;

  nodes++;
#if ME == 2		/* O to move can be drawn */
  if (!r[0])
    return DRAW;
#endif
  for (i = j = 0; i = r[i];) {
    if (THROPP[height[i]] || colthr[columns[i]]) { /* ignore case of colthr */
      if (THROPP[height[i]-8])
        return ILOSE;
      j = rr[0] = i;		/* forget other moves */
      while (i = r[i])
        if (THROPP[height[i]] || colthr[columns[i]])
          return ILOSE;
      break;
    }
    if (!THROPP[height[i]-8])
      j = rr[j] = i;
  }
  if (!j)
    return ILOSE;
  if (j == rr[0]) {
    MAKEMOV(j);
    score = ABOPP(alpha, beta);
    BACKMOV();
    return score;
  }
  if (x = transpose(columns)) {
    score = SCORE(x);
    if (score == DRIN2) {
      if ((beta = DRAW) <= alpha)
        return score;
    } else if (score == DRIN1) {
      if ((alpha = DRAW) >= beta)
        return score;
    } else return score;
  }
  poscnt = posed;
  score = WORST;	/* try to get the best bound if score > beta */
  for (i = rr[j] = 0; rr[i]; ) {
    for (j = i, val = 0; k = rr[j]; j = k) {
      v = KILLER[height[k]];           /* truly dynamic move ordering! */
      if (k == LASTMOVE)		/* make same column attractive */
        v += BONUS;
      if (v > val) {
        val = v;
        l = j;                          /* l ::= predecessor of best */
      }
    }
    if (i != l) {
      rr[l] = rr[k = rr[l]];              /* move best */
      rr[k] = rr[i];
      i = rr[i] = k;
    } else i = rr[i];

    MAKEMOV(i);
    val = ABOPP(alpha, beta);
    BACKMOV();
    if (val IMPRVS score) {
      besti = i;
      if ((score = val) IMPRVS WRSTBND && (WRSTBND = val) NOWORSE BESTBND) {
        if (score == DRAW && rr[i])
          score = NOLOSE;
        break;
      }
    }
  }
  for (j = i = 0; (i = rr[i]) != besti; j++)
    KILLER[height[i]]--;			/* punish bad killers */
  KILLER[height[besti]] += j;
  poscnt = posed - poscnt;
  for (work=1; poscnt>>=1; work++) ;	/* work = log of #positions stored */
  if (x) {
    if (score == DRIN1+DRIN2 - SCORE(x))	/* combine < and > */
      score = DRAW;
    transrestore(columns, score, work);
  } else transtore(columns, score, work);
  return score;
}

#undef ME
#undef OPP
#undef BOOKME
#undef BOOKOPP
#undef ABME
#undef ABOPP
#undef THRME
#undef THROPP
#undef IWIN
#undef ILOSE
#undef MAKEMOV
#undef BACKMOV
#undef WORST
#undef KILLER
#undef WRSTBND
#undef BESTBND
#undef IMPRVS
#undef NOWORSE
#undef NOLOSE
