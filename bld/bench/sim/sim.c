/************************/
/*  sim.c, 27 Oct 1994  */
/*  runtime --> sim.res */
/************************/

/* A PROGRAM FOR LOCAL SIMILARITIES WITH AFFINE WEIGHTS:

Modified 12/6/90 for consistency with the "blast", "convert" and "lav"
programs.

    copyright (c) 1990  Xiaoqiu Huang and Webb Miller

    For the description of the algorithm, please see the paper
    "A Time-Efficient, Linear-Space Local Similarity Algorithm"
	(to appear in Advances in Applied Mathematics)
		Xiaoqiu Huang and Webb Miller
		Department of Computer Science
		The Pennsylvania State University
		University Park, PA 16802

     An early version of this program is presented in the paper
     "A Space-Efficient Algorithm for Local Similarities"
	(to appear in Computer Applications in the Biosciences)
		Xiaoqiu Huang, Ross C. Hardison and Webb Miller
		Department of Computer Science
		Department of Molecular and Cell Biology
		The Pennsylvania State University
		University Park, PA 16802

    The SIM program finds k best non-intersecting alignments between
    two sequences or within one sequence. Using dynamic programming
    techniques, SIM is guaranteed to find optimal alignments. The
    alignments are reported in order of similarity score, with the
    highest scoring alignment first. The k best alignments share no
    aligned pairs. SIM requires space proportional to the sum of the
    input sequence lengths and the output alignment lengths. Thus
    SIM can handle sequences of tens of thousands, even a hundred of
    thousands, of base pairs on a workstation.

    Users can supply values for the parameters:
	M = cost of a matching aligned pair (default = 1)
	I = cost of a transition (default is -1)
	V = cost of a transversion (default is -1)
	O = gap open penalty (default is 6.0)
	E = gap extension penalty (default is 0.2)
    Thus the score for an N-symbol indel is -(Q + R*N).  Values are
    specified with a command argument like O=5.5, where the given number
    is rounded by SIM to the nearest tenth.

    For example, to find 7 best non-intersecting alignments of segments
    from two sequences in files A and B, and using the above default
    values except that transistions are scored 0, use the command

	   sim  7  A  B  I=0

    Acknowledgments
    The functions diff() and display() are from Gene Myers. We made
    the following modifications: similarity weights (integer), instead of
    distance weights (float), are used, the aligned pairs already output
    are not used in the subsequent computation, and the printed alignments
    have been re-formatted in a number of small ways.
*/


#include   <stdio.h>

static char *name1, *name2;             /* names of sequence files    */

#define CMAX 256                        /* for EBCDIC */
/*#define round(x) nint(x)              /* round to larger magnitude */
#define round(x) ((x)>0.0 ? (x)+0.5 : (x)-0.5)

main(argc, argv) int argc; char *argv[];
{ long  M, N, K;                        /* Sequence lengths and k     */
  char  *A,  *B;                        /* Storing two sequences      */
  int symbol;
  long V[CMAX][CMAX], Q, R;             /* Converted integer weights  */
  double starttime, benchtime, dtime();
  double parm_M, parm_I, parm_V, parm_O, parm_E, v;
  double atof();
  FILE *Bp, *Ap, *Cp, *ckopen();
  char *ckalloc();                      /* space-allocating function  */

  if ((Cp = fopen("sim.res","a+")) == NULL)
    {
	printf("Can not open sim.res\n\n");
	exit(1);
    }

  starttime = dtime();      
  
	if ( argc < 4)
		fatal("SIM k file1 file2 [M=] [I=] [V=] [O=] [E=]");

	/* read k: the number of local alignments to find */
	sscanf(argv[1],"%d", &K);
	if (K == 0)
		fatal("specified 0 alignments");

	/* determine the sequence lengths */
	Ap = ckopen(argv[2], "r");
	for (M = 0; ( symbol = getc(Ap)) != EOF ; )
	   if ( symbol != '\n' )
		++M;
	fclose(Ap);
	name1 = argv[2];

	/* allocate space for A */
	A = ( char * ) ckalloc( (M + 1) * sizeof(char));

	/* read the first sequence into A */
	Ap = ckopen(argv[2], "r");
	for (M = 0; ( symbol = getc(Ap)) != EOF ; )
	   if ( symbol != '\n' )
		A[++M] = symbol;

	if (strcmp(argv[2],argv[3])) {  /* sequences are different */
		/* read the second sequence into B */
		Bp = ckopen(argv[3], "r");
		for (N = 0; ( symbol = getc(Bp)) != EOF ; )
			if ( symbol != '\n' )
				++N;
		fclose(Bp);
		name2 = argv[3];
		B = ( char * ) ckalloc( (N + 1) * sizeof(char));
		Bp = ckopen(argv[3], "r");
		for (N = 0; ( symbol = getc(Bp)) != EOF ; )
			if ( symbol != '\n' )
				B[++N] = symbol;
	}

	parm_M = 1.0;
	parm_I = -1.0;
	parm_V = -1.0;
	parm_O = 6.0;
	parm_E = 0.2;
	while (--argc > 3) {
		if (argv[argc][1] != '=')
			fatalf("argument %d has improper form", argc);
		v = atof(argv[argc]+2);
		switch (argv[argc][0]) {
			case 'M': parm_M = v; break;
			case 'I': parm_I = v; break;
			case 'V': parm_V = v; break;
			case 'O': parm_O = v; break;
			case 'E': parm_E = v; break;
			default: fatal("options are M, I, V, O and E.");
		}
	}
	printf("\t\tSIM output with parameters:\n");
	printf("\t\tM = %g, I = %g, V = %g\n\t\tO = %g, E = %g\n\n",
	   parm_M, parm_I, parm_V, parm_O, parm_E);

	/* set up scoring matrix */
	V['A']['A'] = V['C']['C'] = 
	V['G']['G'] = V['T']['T'] = round(10*parm_M);

	V['A']['G'] = V['G']['A'] = 
	V['C']['T'] = V['T']['C'] = round(10*parm_I);

	V['A']['C'] = V['A']['T'] = 
	V['C']['A'] = V['C']['G'] =
	V['G']['C'] = V['G']['T'] = 
	V['T']['A'] = V['T']['G'] = round(10*parm_V);

	Q = round (10*parm_O);
	R = round (10*parm_E);

	if (strcmp(argv[2], argv[3]))
		SIM(A,B,M,N,K,V,Q,R,2L);
	else
		SIM(A,A,M,M,K,V,Q,R,1L);

  benchtime = dtime() - starttime;
  printf("\nRun Time (sec) = %9.1lf\n",benchtime);
  
  fprintf(Cp,"\n");
  fprintf(Cp,"  Run Line: sim 8 tob.38-44 liv.42-48\n");
  fprintf(Cp,"  Run Time: %9.1lf (sec)\n",benchtime);
  fprintf(Cp,"#######################################################\n");
  fclose(Cp);

	exit(0);
}

static long (*v)[CMAX];                 /* substitution scores */
static long q, r;                       /* gap penalties */
static long qr;                         /* qr = q + r */

typedef struct ONE { long COL ;  struct ONE  *NEXT ;} pair, *pairptr;
pairptr *row, z;                      /* for saving used aligned pairs */
static short tt;

typedef struct NODE
	{ long  SCORE;
	  long  STARI;
	  long  STARJ;
	  long  ENDI;
	  long  ENDJ;
	  long  TOP;
	  long  BOT;
	  long  LEFT;
	  long  RIGHT; }  vertex, *vertexptr;
		
vertexptr  *LIST;                  /* an array for saving k best scores */
vertexptr  low = 0;                /* lowest score node in LIST */
vertexptr  most = 0;               /* latestly accessed node in LIST */
static long numnode;               /* the number of nodes in LIST */

static long *CC, *DD;              /* saving matrix scores */
static long *RR, *SS, *EE, *FF;    /* saving start-points */
static long *HH, *WW;              /* saving matrix scores */
static long *II, *JJ, *XX, *YY;    /* saving start-points */
static long  m1, mm, n1, nn;       /* boundaries of recomputed area */
static long  rl, cl;               /* left and top boundaries */
static long  min;                  /* minimum score in LIST */
static short flag;                /* indicate if recomputation necessary*/

/* DIAG() assigns value to x if (ii,jj) is never used before */
#define DIAG(ii, jj, x, value)                         \
{ for ( tt = 1, z = row[(ii)]; z != 0; z = z->NEXT )   \
    if ( z->COL == (jj) )                              \
	{ tt = 0; break; }                               \
  if ( tt )                                            \
    x = ( value );                                     \
}                                                     

/* replace (ss1, xx1, yy1) by (ss2, xx2, yy2) if the latter is large */
#define ORDER(ss1, xx1, yy1, ss2, xx2, yy2)             \
{ if ( ss1 < ss2 )                                      \
    { ss1 = ss2; xx1 = xx2; yy1 = yy2; }                \
  else                                                  \
    if ( ss1 == ss2 )                                   \
	{ if ( xx1 < xx2 )                                \
	  { xx1 = xx2; yy1 = yy2; }                     \
	else                                            \
	  if ( xx1 == xx2 && yy1 < yy2 )                \
	    yy1 = yy2;                                  \
	}                                                 \
}

/* The following definitions are for function diff() */

long  diff(), display();
static long  zero = 0;                     /* long type zero */

#define gap(k)  ((k) <= 0 ? 0 : q+r*(k))   /* k-symbol indel score */

static long *sapp;                         /* Current script append ptr */
static long  last;                         /* Last script op appended */

static long I, J;                          /* current positions of A ,B */
static long no_mat;                        /* number of matches */ 
static long no_mis;                        /* number of mismatches */ 
static long al_len;                        /* length of alignment */
					   /* Append "Delete k" op */
#define DEL(k)                          \
{ I += k;                               \
  al_len += k;                          \
  if (last < 0)                         \
    last = sapp[-1] -= (k);             \
  else                                  \
    last = *sapp++ = -(k);              \
}
						/* Append "Insert k" op */
#define INS(k)                          \
{ J += k;                               \
  al_len += k;                          \
  if (last < 0)                         \
    { sapp[-1] = (k); *sapp++ = last; } \
  else                                  \
    last = *sapp++ = (k);               \
}

						/* Append "Replace" op */
#define REP                             \
{ last = *sapp++ = 0;                   \
  al_len += 1;                          \
}

/* SIM(A,B,M,N,K,V,Q,R) reports K best non-intersecting alignments of
   the segments of A and B in order of similarity scores, where
   V[a][b] is the score of aligning a and b, and -(Q+R*i) is the score
   of an i-symbol indel.                                               */

SIM(A,B,M,N,K,V,Q,R,nseq) char A[],B[]; long M,N,K,V[][CMAX],Q,R,nseq;
{ long endi, endj, stari, starj;        /* endpoint and startpoint */ 
  long  score;                          /* the max score in LIST */
  long count;                           /* maximum size of list */      
  register  long  i, j;                 /* row and column indices */
  char *ckalloc();                      /* space-allocating function */
  long  *S;                             /* saving operations for diff */
  vertexptr cur;                        /* temporary pointer */
  vertexptr findmax();                 /* return the largest score node */
	
	/* allocate space for all vectors */
	j = (N + 1) * sizeof(long);
	CC = ( long * ) ckalloc(j);
	DD = ( long * ) ckalloc(j);
	RR = ( long * ) ckalloc(j);
	SS = ( long * ) ckalloc(j);
	EE = ( long * ) ckalloc(j);
	FF = ( long * ) ckalloc(j);
	i = (M + 1) * sizeof(long);
	HH = ( long * ) ckalloc(i);
	WW = ( long * ) ckalloc(i);
	II = ( long * ) ckalloc(i);
	JJ = ( long * ) ckalloc(i);
	XX = ( long * ) ckalloc(i);
	YY = ( long * ) ckalloc(i);
	S = ( long * ) ckalloc(i + j);
	row = ( pairptr * ) ckalloc( (M + 1) * sizeof(pairptr));

	/* set up list for each row */
	for ( i = 1; i <= M; i++ )
	  if ( nseq == 2 )
	     row[i] = 0;
	  else
	    { row[i] = z = ( pairptr ) ckalloc( (long) sizeof(pair));
		z->COL = i;                       
		z->NEXT = 0;
	    }

	v = V;
	q = Q;
	r = R;
	qr = q + r;

	LIST = ( vertexptr * ) ckalloc( K * sizeof(vertexptr));
	for ( i = 0; i < K ; i++ )
	   LIST[i] = ( vertexptr ) ckalloc( (long) sizeof(vertex));

	printf("                 Upper Sequence : %s\n", name1);
	printf("                         Length : %d\n", M);
	printf("                 Lower Sequence : %s\n", name2);
	printf("                         Length : %d\n", N);

	numnode = min = 0;
	big_pass(A,B,M,N,K,nseq);

	/* Report the K best alignments one by one. After each
	   alignment is output, recompute part of the matrix. First
	   determine the size of the area to be recomputed, then do the
	   recomputation     */

	for ( count = K - 1; count >= 0 ; count-- )
	  { if ( numnode == 0 )
		fatal("The number of alignments computed is too large");
	    cur = findmax();/* Return a pointer to a node with max score*/
	    score = cur->SCORE;
	    stari = ++cur->STARI;
	    starj = ++cur->STARJ;
	    endi = cur->ENDI;
	    endj = cur->ENDJ;
	    m1 = cur->TOP;
	    mm = cur->BOT;
	    n1 = cur->LEFT;
	    nn = cur->RIGHT;
	    rl = endi - stari + 1;
	    cl = endj - starj + 1;
	    I = stari - 1;
	    J = starj - 1;
	    sapp = S;
	    last = 0;
	    al_len = 0;
	    no_mat = 0;
	    no_mis = 0;
	    diff(&A[stari]-1, &B[starj]-1,rl,cl,q,q);
	    /* Output the best alignment */
  printf("\n*********************************************************\n");
  printf("      Number %d Local Alignment\n", K - count);
  printf("      Similarity Score : %g\n",score/10.0);
  printf("      Match Percentage : %d%%\n", (100*no_mat)/al_len);
  printf("      Number of Matches : %d%\n", no_mat);
  printf("      Number of Mismatches : %d%\n", no_mis);
  printf("      Total Length of Gaps : %d%\n", al_len-no_mat-no_mis);
  printf("      Begins at (%d, %d) and Ends at (%d, %d)\n",
			stari,starj, endi,endj);
	    display(&A[stari]-1,&B[starj]-1,rl,cl,S,stari,starj);
	    fflush(stdout);

	    if ( count )
		{ flag = 0;
		locate(A,B,nseq);
		if ( flag )
		   small_pass(A,B,count,nseq);
		}
	  }
}

/* A big pass to compute K best classes */

big_pass(A,B,M,N,K,nseq) char A[],B[]; long M,N,K,nseq;
{ register  long  i, j;           /* row and column indices */
  register  long  c;              /* best score at current point */
  register  long  f;              /* best score ending with insertion */
  register  long  d;              /* best score ending with deletion */
  register  long  p;              /* best score at (i-1, j-1) */
  register  long  ci, cj;         /* end-point associated with c */ 
  register  long  di, dj;         /* end-point associated with d */
  register  long  fi, fj;               /* end-point associated with f */
  register  long  pi, pj;               /* end-point associated with p */
  long  *va;                            /* pointer to v(A[i], B[j]) */
  long   addnode();                     /* function for inserting a node */

	
	/* Compute the matrix and save the top K best scores in LIST
	   CC : the scores of the current row
	   RR and EE : the starting point that leads to score CC
	   DD : the scores of the current row, ending with deletion
	   SS and FF : the starting point that leads to score DD        */
	/* Initialize the 0 th row */
	for ( j = 1; j <= N ; j++ )
	  {  CC[j] = 0;
	     RR[j] = 0;
	     EE[j] = j;
	     DD[j] = - (q);
	     SS[j] = 0;
	     FF[j] = j;
	  }
	for ( i = 1; i <= M; i++) 
	  {  c = 0;                             /* Initialize column 0 */
	     f = - (q);
	     ci = fi = i;
	     va = v[A[i]];
	     if ( nseq == 2 )
		 { p = 0;
		 pi = i - 1;
		 cj = fj = pj = 0;
		 }
	     else
		 { p = CC[i];
		 pi = RR[i];
		 pj = EE[i];
		 cj = fj = i;
		 }
	     for ( j = (nseq == 2 ? 1 : (i+1)) ; j <= N ; j++ )  
		 {  f = f - r;
		  c = c - qr;
		  ORDER(f, fi, fj, c, ci, cj)
		  c = CC[j] - qr; 
		  ci = RR[j];
		  cj = EE[j];
		  d = DD[j] - r;
		  di = SS[j];
		  dj = FF[j];
		  ORDER(d, di, dj, c, ci, cj)
		  c = 0;
		  DIAG(i, j, c, p+va[B[j]])             /* diagonal */
		  if ( c <= 0 )
		    { c = 0; ci = i; cj = j; }
		  else
		    { ci = pi; cj = pj; }
		  ORDER(c, ci, cj, d, di, dj)
		  ORDER(c, ci, cj, f, fi, fj)
		  p = CC[j];
		  CC[j] = c;
		  pi = RR[j];
		  pj = EE[j];
		  RR[j] = ci;
		  EE[j] = cj;
		  DD[j] = d;
		  SS[j] = di;
		  FF[j] = dj;
		  if ( c > min )        /* add the score into list */
		    min = addnode(c, ci, cj, i, j, K, min);
		}
	  }
}

/* Determine the left and top boundaries of the recomputed area */

locate(A,B,nseq) char A[],B[]; long nseq;
{ register  long  i, j;             /* row and column indices */
  register  long  c;                /* best score at current point */
  register  long  f;                /* best score ending with insertion */
  register  long  d;                /* best score ending with deletion */
  register  long  p;                /* best score at (i-1, j-1) */
  register  long  ci, cj;           /* end-point associated with c */ 
  register  long  di, dj;           /* end-point associated with d */
  register  long  fi, fj;           /* end-point associated with f */
  register  long  pi, pj;           /* end-point associated with p */
  short  cflag, rflag;              /* for recomputation */
  long  *va;                        /* pointer to v(A[i], B[j]) */
  long   addnode();                 /* function for inserting a node */
  long  limit;                      /* the bound on j */

	/* Reverse pass
	   rows
	   CC : the scores on the current row
	   RR and EE : the endpoints that lead to CC
	   DD : the deletion scores 
	   SS and FF : the endpoints that lead to DD

	   columns
	   HH : the scores on the current columns
	   II and JJ : the endpoints that lead to HH
	   WW : the deletion scores
	   XX and YY : the endpoints that lead to WW
	*/
	for ( j = nn; j >= n1 ; j-- )
	  {  CC[j] = 0;
	     EE[j] = j;
	     DD[j] = - (q);
	     FF[j] = j;
	     if ( nseq == 2 || j > mm )
		RR[j] = SS[j] = mm + 1;
	     else
		RR[j] = SS[j] = j;
	  }

	for ( i = mm; i >= m1; i-- )
	  {  c = p = 0;
	     f = - (q);
	     ci = fi = i;
	     pi = i + 1;
	     cj = fj = pj = nn + 1;
	     va = v[A[i]];
	     if ( nseq == 2 || n1 > i )
		limit = n1;
	     else
		limit = i + 1;
	     for ( j = nn; j >= limit ; j-- )  
		 {  f = f - r;
		  c = c - qr;
		  ORDER(f, fi, fj, c, ci, cj)
		  c = CC[j] - qr; 
		  ci = RR[j];
		  cj = EE[j];
		  d = DD[j] - r;
		  di = SS[j];
		  dj = FF[j];
		  ORDER(d, di, dj, c, ci, cj)
		  c = 0;
		  DIAG(i, j, c, p+va[B[j]])             /* diagonal */
		  if ( c <= 0 )
		    { c = 0; ci = i; cj = j; }
		  else
		    { ci = pi; cj = pj; }
		  ORDER(c, ci, cj, d, di, dj)
		  ORDER(c, ci, cj, f, fi, fj)
		  p = CC[j];
		  CC[j] = c;
		  pi = RR[j];
		  pj = EE[j];
		  RR[j] = ci;
		  EE[j] = cj;
		  DD[j] = d;
		  SS[j] = di;
		  FF[j] = dj;
		  if ( c > min )
		    flag = 1;
		}
	     if ( nseq == 2 || i < n1 )
		 { HH[i] = CC[n1];
		 II[i] = RR[n1];
		 JJ[i] = EE[n1];
		 WW[i] = DD[n1];
		 XX[i] = SS[n1];
		 YY[i] = FF[n1];
		 }
	  }
	
  for ( rl = m1, cl = n1; ; )
    { for ( rflag = cflag = 1; (rflag && m1 > 1) || ( cflag && n1 > 1);)
	{ if ( rflag && m1 > 1 )        /* Compute one row */
	    { rflag = 0;
		m1--;
		c = p = 0;
		f = - (q);
		ci = fi = m1;
		pi = m1 + 1;
		cj = fj = pj = nn + 1;
		va = v[A[m1]];
		for ( j = nn; j >= n1 ; j-- )  
		{ f = f - r;
		  c = c - qr;
		  ORDER(f, fi, fj, c, ci, cj)
		  c = CC[j] - qr; 
		  ci = RR[j];
		  cj = EE[j];
		  d = DD[j] - r;
		  di = SS[j];
		  dj = FF[j];
		  ORDER(d, di, dj, c, ci, cj)
		  c = 0;
		  DIAG(m1, j, c, p+va[B[j]])            /* diagonal */
		  if ( c <= 0 )
		    { c = 0; ci = m1; cj = j; }
		  else
		    { ci = pi; cj = pj; }
		  ORDER(c, ci, cj, d, di, dj)
		  ORDER(c, ci, cj, f, fi, fj)
		  p = CC[j];
		  CC[j] = c;
		  pi = RR[j];
		  pj = EE[j];
		  RR[j] = ci;
		  EE[j] = cj;
		  DD[j] = d;
		  SS[j] = di;
		  FF[j] = dj;
		  if ( c > min )
		     flag = 1;
		  if ( ! rflag && (ci > rl && cj > cl || di > rl && dj>cl
						  || fi > rl && fj > cl))
			rflag = 1;
		}
		HH[m1] = CC[n1];
		II[m1] = RR[n1];
		JJ[m1] = EE[n1];
		WW[m1] = DD[n1];
		XX[m1] = SS[n1];
		YY[m1] = FF[n1];
		if ( ! cflag && ( ci > rl && cj > cl || di > rl && dj > cl
			     || fi > rl && fj > cl ) )
		 cflag = 1;
	    }

	  if ( nseq == 1 && n1 == (m1 + 1) && ! rflag )
	     cflag = 0;
	  if ( cflag && n1 > 1 )        /* Compute one column */
	    { cflag = 0;
		n1--;
		c = 0;
		f = - (q);
		cj = fj = n1;
		va = v[B[n1]];
		if ( nseq == 2 || mm < n1 )
		{ p = 0;
		  ci = fi = pi = mm + 1;
		  pj = n1 + 1;
		  limit = mm;
		}
		else
		{ p = HH[n1];
		  pi = II[n1];
		  pj = JJ[n1];
		  ci = fi = n1;
		  limit = n1 - 1;
		}
		for ( i = limit; i >= m1 ; i-- )  
		{ f = f - r;
		  c = c - qr;
		  ORDER(f, fi, fj, c, ci, cj)
		  c = HH[i] - qr; 
		  ci = II[i];
		  cj = JJ[i];
		  d = WW[i] - r;
		  di = XX[i];
		  dj = YY[i];
		  ORDER(d, di, dj, c, ci, cj)
		  c = 0;
		  DIAG(i, n1, c, p+va[A[i]])
		  if ( c <= 0 )
		    { c = 0; ci = i; cj = n1; }
		  else
		    { ci = pi; cj = pj; }
		  ORDER(c, ci, cj, d, di, dj)
		  ORDER(c, ci, cj, f, fi, fj)
		  p = HH[i];
		  HH[i] = c;
		  pi = II[i];
		  pj = JJ[i];
		  II[i] = ci;
		  JJ[i] = cj;
		  WW[i] = d;
		  XX[i] = di;
		  YY[i] = dj;
		  if ( c > min )
		     flag = 1;
		  if (! cflag && (ci > rl && cj > cl || di > rl && dj >cl
				 || fi > rl && fj > cl ) )
		     cflag = 1;
		}
		CC[n1] = HH[m1];
		RR[n1] = II[m1];
		EE[n1] = JJ[m1];
		DD[n1] = WW[m1];
		SS[n1] = XX[m1];
		FF[n1] = YY[m1];
		if ( ! rflag && ( ci > rl && cj > cl || di > rl && dj > cl
						 || fi > rl && fj > cl ))
		 rflag = 1;
	    }
	}
	if ( m1 == 1 && n1 == 1 || no_cross() )
	 break;
   }
  m1--;
  n1--;
}

/* recompute the area on forward pass */
small_pass(A,B,count,nseq) char A[], B[]; long count, nseq;
{ register  long  i, j;             /* row and column indices */
  register  long  c;                /* best score at current point */
  register  long  f;                /* best score ending with insertion */
  register  long  d;                /* best score ending with deletion */
  register  long  p;                /* best score at (i-1, j-1) */
  register  long  ci, cj;           /* end-point associated with c */ 
  register  long  di, dj;           /* end-point associated with d */
  register  long  fi, fj;           /* end-point associated with f */
  register  long  pi, pj;           /* end-point associated with p */
  long  *va;                        /* pointer to v(A[i], B[j]) */
  long   addnode();                 /* function for inserting a node */
  long  limit;                      /* lower bound on j */

	for ( j = n1 + 1; j <= nn ; j++ )
	  {  CC[j] = 0;
	     RR[j] = m1;
	     EE[j] = j;
	     DD[j] = - (q);
	     SS[j] = m1;
	     FF[j] = j;
	  }
	for ( i = m1 + 1; i <= mm; i++) 
	  {  c = 0;                             /* Initialize column 0 */
	     f = - (q);
	     ci = fi = i;
	     va = v[A[i]];
	     if ( nseq == 2 || i <= n1 )
		 { p = 0;
		 pi = i - 1;
		 cj = fj = pj = n1;
		 limit = n1 + 1;
		 }
	     else
		 { p = CC[i];
		 pi = RR[i];
		 pj = EE[i];
		 cj = fj = i;
		 limit = i + 1;
		 }
	     for ( j = limit ; j <= nn ; j++ )  
		 {  f = f - r;
		  c = c - qr;
		  ORDER(f, fi, fj, c, ci, cj)
		  c = CC[j] - qr; 
		  ci = RR[j];
		  cj = EE[j];
		  d = DD[j] - r;
		  di = SS[j];
		  dj = FF[j];
		  ORDER(d, di, dj, c, ci, cj)
		  c = 0;
		  DIAG(i, j, c, p+va[B[j]])             /* diagonal */
		  if ( c <= 0 )
		    { c = 0; ci = i; cj = j; }
		  else
		    { ci = pi; cj = pj; }
		  ORDER(c, ci, cj, d, di, dj)
		  ORDER(c, ci, cj, f, fi, fj)
		  p = CC[j];
		  CC[j] = c;
		  pi = RR[j];
		  pj = EE[j];
		  RR[j] = ci;
		  EE[j] = cj;
		  DD[j] = d;
		  SS[j] = di;
		  FF[j] = dj;
		  if ( c > min )        /* add the score into list */
		    min = addnode(c, ci, cj, i, j, count, min);
		}
	  }
}

/* Add a new node into list.  */

long addnode(c, ci, cj, i, j, K, cost)  long c, ci, cj, i, j, K, cost;
{ short found;                          /* 1 if the node is in LIST */
  register long d;

  found = 0;
  if ( most != 0 && most->STARI == ci && most->STARJ == cj )
    found = 1;
  else
     for ( d = 0; d < numnode ; d++ )
	{ most = LIST[d];
	  if ( most->STARI == ci && most->STARJ == cj )
	    { found = 1;
		break;
	    }
	}
  if ( found )
    { if ( most->SCORE < c )
	{ most->SCORE = c;
	  most->ENDI = i;
	  most->ENDJ = j;
	}
	if ( most->TOP > i ) most->TOP = i;
	if ( most->BOT < i ) most->BOT = i;
	if ( most->LEFT > j ) most->LEFT = j;
	if ( most->RIGHT < j ) most->RIGHT = j;
    }
  else
    { if ( numnode == K )       /* list full */
	 most = low;
	else
	 most = LIST[numnode++];
	most->SCORE = c;
	most->STARI = ci;
	most->STARJ = cj;
	most->ENDI = i;
	most->ENDJ = j;
	most->TOP = most->BOT = i;
	most->LEFT = most->RIGHT = j;
    }
  if ( numnode == K )
    { if ( low == most || ! low ) 
	{ for ( low = LIST[0], d = 1; d < numnode ; d++ )
	    if ( LIST[d]->SCORE < low->SCORE )
		low = LIST[d];
	}
	return ( low->SCORE ) ;
    }
  else
    return cost;
}

/* Find and remove the largest score in list */

vertexptr findmax()
{ vertexptr  cur;
  register long i, j;

  for ( j = 0, i = 1; i < numnode ; i++ )
    if ( LIST[i]->SCORE > LIST[j]->SCORE )
	 j = i;
  cur = LIST[j];
  if ( j != --numnode )
    { LIST[j] = LIST[numnode];
	LIST[numnode] =  cur;
    }
  most = LIST[0];
  if ( low == cur ) low = LIST[0];
  return ( cur );
}

/* return 1 if no node in LIST share vertices with the area */

no_cross()
{ vertexptr  cur;
  register long i;

	for ( i = 0; i < numnode; i++ )
	{ cur = LIST[i];
	  if ( cur->STARI <= mm && cur->STARJ <= nn && cur->BOT >= m1-1 && 
		 cur->RIGHT >= n1-1 && (cur->STARI < rl || cur->STARJ < cl))
	     { if ( cur->STARI < rl ) rl = cur->STARI;
		 if ( cur->STARJ < cl ) cl = cur->STARJ;
		 flag = 1;
		 break;
	     }
	}
	if ( i == numnode )
	return 1;
	else
	return 0;
}

/* diff(A,B,M,N,tb,te) returns the score of an optimum conversion between
   A[1..M] and B[1..N] that begins(ends) with a delete if tb(te) is zero
   and appends such a conversion to the current script.                */

long diff(A,B,M,N,tb,te) char *A, *B; long M, N; long tb, te;

{ long   midi, midj, type;      /* Midpoint, type, and cost */
  long midc;

{ register long   i, j;
  register long c, e, d, s;
	   long t, *va;
	   char  *ckalloc();

/* Boundary cases: M <= 1 or N == 0 */

  if (N <= 0)
    { if (M > 0) DEL(M)
	return - gap(M);
    }
  if (M <= 1)
    { if (M <= 0)
	{ INS(N);
	  return - gap(N);
	}
	if (tb > te) tb = te;
	midc = - (tb + r + gap(N) );
	midj = 0;
	va = v[A[1]];
	for (j = 1; j <= N; j++)
	{  for ( tt = 1, z = row[I+1]; z != 0; z = z->NEXT )    
		if ( z->COL == j+J )                      
		 { tt = 0; break; }             
	   if ( tt )                    
	    { c = va[B[j]] - ( gap(j-1) + gap(N-j) );
		if (c > midc)
		 { midc = c;
		 midj = j;
		 }
	    }
	}
	if (midj == 0)
	{ INS(N) DEL(1) }
	else
	{ if (midj > 1) INS(midj-1)
	  REP
	  if ( A[1] == B[midj] )
	     no_mat += 1;
	  else
	     no_mis += 1;
	  /* mark (A[I],B[J]) as used: put J into list row[I] */        
	  I++; J++;
	  z = ( pairptr ) ckalloc( (long) sizeof(pair));
	  z->COL = J;                   
	  z->NEXT = row[I];                             
	  row[I] = z;
	  if (midj < N) INS(N-midj)
	}
	return midc;
    }

/* Divide: Find optimum midpoint (midi,midj) of cost midc */

  midi = M/2;             /* Forward phase:                          */
  CC[0] = 0;              /*   Compute C(M/2,k) & D(M/2,k) for all k */
  t = -q;
  for (j = 1; j <= N; j++)
    { CC[j] = t = t-r;
	DD[j] = t-q;
    }
  t = -tb;
  for (i = 1; i <= midi; i++)
    { s = CC[0];
	CC[0] = c = t = t-r;
	e = t-q;
	va = v[A[i]];
	for (j = 1; j <= N; j++)
	{ if ((c = c - qr) > (e = e - r)) e = c;
	  if ((c = CC[j] - qr) > (d = DD[j] - r)) d = c;
	  DIAG(i+I, j+J, c, s+va[B[j]])
	  if (c < d) c = d;
	  if (c < e) c = e;
	  s = CC[j];
	  CC[j] = c;
	  DD[j] = d;
	}
    }
  DD[0] = CC[0];

  RR[N] = 0;              /* Reverse phase:                          */
  t = -q;                 /*   Compute R(M/2,k) & S(M/2,k) for all k */
  for (j = N-1; j >= 0; j--)
    { RR[j] = t = t-r;
	SS[j] = t-q;
    }
  t = -te;
  for (i = M-1; i >= midi; i--)
    { s = RR[N];
	RR[N] = c = t = t-r;
	e = t-q;
	va = v[A[i+1]];
	for (j = N-1; j >= 0; j--)
	{ if ((c = c - qr) > (e = e - r)) e = c;
	  if ((c = RR[j] - qr) > (d = SS[j] - r)) d = c;
	  DIAG(i+1+I, j+1+J, c, s+va[B[j+1]])
	  if (c < d) c = d;
	  if (c < e) c = e;
	  s = RR[j];
	  RR[j] = c;
	  SS[j] = d;
	}
    }
  SS[N] = RR[N];

  midc = CC[0]+RR[0];           /* Find optimal midpoint */
  midj = 0;
  type = 1;
  for (j = 0; j <= N; j++)
    if ((c = CC[j] + RR[j]) >= midc)
	if (c > midc || CC[j] != DD[j] && RR[j] == SS[j])
	{ midc = c;
	  midj = j;
	}
  for (j = N; j >= 0; j--)
    if ((c = DD[j] + SS[j] + q) > midc)
	{ midc = c;
	midj = j;
	type = 2;
	}
}

/* Conquer: recursively around midpoint */

  if (type == 1)
    { diff(A,B,midi,midj,tb,q);
	diff(A+midi,B+midj,M-midi,N-midj,q,te);
    }
  else
    { diff(A,B,midi-1,midj,tb,zero);
	DEL(2);
	diff(A+midi+1,B+midj,M-midi-1,N-midj,zero,te);
    }
  return midc;
}

/* Alignment display routine */

static char ALINE[51], BLINE[51], CLINE[51];

long display(A,B,M,N,S,AP,BP) char A[], B[]; long M, N; long S[], AP, BP;
{ register char *a, *b, *c;
  register long   i,  j, op;
	   long   lines, ap, bp;
	   char x, y, z;


  i = j = op = lines = 0;
  ap = AP;
  bp = BP;
  a = ALINE;
  b = BLINE;
  c = CLINE;
  while (i < M || j < N)
    { if (op == 0 && *S == 0)
	{ op = *S++;
	  *a = A[++i];
	  *b = B[++j];
	  x = *a++;
	  y = *b++;
	  z = ' ';
	  if (x == y)
		z = '|';
	  if (
		(x == 'A' && y == 'G') ||
		(x == 'C' && y == 'T') ||
		(x == 'G' && y == 'A') ||
		(x == 'T' && y == 'C')
	  ) z = ':';
	  *c++ = z;
	}
	else
	{ if (op == 0)
	    op = *S++;
	  if (op > 0)
	    { *a++ = ' ';
		*b++ = B[++j];
		op--;
	    }
	  else
	    { *a++ = A[++i];
		*b++ = ' ';
		op++;
	    }
	  *c++ = '-';
	}
	if (a >= ALINE+50 || i >= M && j >= N)
	{ *a = *b = *c = '\0';
	  printf("\n%5d ",50*lines++);
	  for (b = ALINE+10; b <= a; b += 10)
	    printf("    .    :");
	  if (b <= a+5)
	    printf("    .");
	  printf("\n%5d %s\n      %s\n%5d %s\n",ap,ALINE,CLINE,bp,BLINE);
	  ap = AP + i;
	  bp = BP + j;
	  a = ALINE;
	  b = BLINE;
	  c = CLINE;
	}
    }
}

/* lib.c - library of C procedures. */

/* fatal - print message and die */
fatal(msg)
char *msg;
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/* fatalf - format message, print it, and die */
fatalf(msg, val)
char *msg, *val;
{
	fprintf(stderr, msg, val);
	putc('\n', stderr);
	exit(1);
}
	
/* ckopen - open file; check for success */
FILE *ckopen(name, mode)
char *name, *mode;
{
	FILE *fopen(), *fp;

	if ((fp = fopen(name, mode)) == NULL)
		fatalf("Cannot open %s.", name);
	return(fp);
}

/* ckalloc - allocate space; check for success */
char *ckalloc(amount)
long amount;
{
	char *malloc(), *p;

	if ((p = malloc( (unsigned) amount)) == NULL)
		fatal("Ran out of memory.");
	return(p);
}

/*****************************************************/
/* Various timer routines.                           */
/* Al Aburto, aburto@marlin.nosc.mil, 16 Dec 1995    */
/*                                                   */
/* t = dtime() outputs the current time in seconds.  */
/* Use CAUTION as some of these routines will mess   */
/* up when timing across the hour mark!!!            */
/*                                                   */
/* For timing I use the 'user' time whenever         */
/* possible. Using 'user+sys' time is a separate     */
/* issue.                                            */
/*                                                   */
/* Example Usage:                                    */
/* [timer options added here]                        */
/* main()                                            */
/* {                                                 */
/* double starttime,benchtime,dtime();               */
/*                                                   */
/* starttime = dtime();                              */ 
/* [routine to time]                                 */
/* benchtime = dtime() - starttime;                  */
/* }                                                 */
/*                                                   */
/* [timer code below added here]                     */
/*****************************************************/

/*******************/
/*  Amiga dtime()  */
/*******************/
#ifdef Amiga
#include <ctype.h>
#define HZ 50

double dtime()
{
	double q;

	struct   tt {
		long  days;
		long  minutes;
		long  ticks;
	} tt;

	DateStamp(&tt);

	q = ((double)(tt.ticks + (tt.minutes * 60L * 50L))) / (double)HZ;

	return q;
}
#endif

/*****************************************************/
/*  UNIX dtime(). This is the preferred UNIX timer.  */
/*  Provided by: Markku Kolkka, mk59200@cc.tut.fi    */
/*  HP-UX Addition by: Bo Thide', bt@irfu.se         */
/*****************************************************/
#ifdef UNIX
#include <sys/time.h>
#include <sys/resource.h>

#ifdef hpux
#include <sys/syscall.h>
#define getrusage(a,b) syscall(SYS_getrusage,a,b)
#endif

struct rusage rusage;

double dtime()
{
	double q;

	getrusage(RUSAGE_SELF,&rusage);

	q = (double)(rusage.ru_utime.tv_sec);
	q = q + (double)(rusage.ru_utime.tv_usec) * 1.0e-06;
	
	return q;
}
#endif

/***************************************************/
/*  UNIX_Old dtime(). This is the old UNIX timer.  */
/*  Use only if absolutely necessary as HZ may be  */
/*  ill defined on your system.                    */
/***************************************************/
#ifdef UNIX_Old
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>

#ifndef HZ
#define HZ 60
#endif

struct tms tms;

double dtime()
{
	double q;

	times(&tms);

	q = (double)(tms.tms_utime) / (double)HZ;
	
	return q;
}
#endif

/*********************************************************/
/*  VMS dtime() for VMS systems.                         */
/*  Provided by: RAMO@uvphys.phys.UVic.CA                */
/*  Some people have run into problems with this timer.  */
/*********************************************************/
#ifdef VMS
#include time

#ifndef HZ
#define HZ 100
#endif

struct tbuffer_t
		 {
	int proc_user_time;
	int proc_system_time;
	int child_user_time;
	int child_system_time;
		 };
struct tbuffer_t tms;

double dtime()
{
	double q;

	times(&tms);

	q = (double)(tms.proc_user_time) / (double)HZ;
	
	return q;
}
#endif

/******************************/
/*  BORLAND C dtime() for DOS */
/******************************/
#ifdef BORLAND_C
#include <ctype.h>
#include <dos.h>
#include <time.h>

#define HZ 100
struct time tnow;

double dtime()
{
	double q;

	gettime(&tnow);

	q = 60.0 * (double)(tnow.ti_min);
	q = q + (double)(tnow.ti_sec);
	q = q + (double)(tnow.ti_hund)/(double)HZ;
	
	return q;
}
#endif

/**************************************/
/*  Microsoft C (MSC) dtime() for DOS */
/**************************************/
#ifdef MSC
#include <time.h>
#include <ctype.h>

#define HZ CLOCKS_PER_SEC
clock_t tnow;

double dtime()
{
	double q;

	tnow = clock();

	q = (double)tnow / (double)HZ;
	
	return q;
}
#endif

/*************************************/
/*  Macintosh (MAC) Think C dtime()  */
/*************************************/
#ifdef MAC
#include <time.h>

#define HZ 60

double dtime()
{
	double q;

	q = (double)clock() / (double)HZ;
	
	return q;
}
#endif

/************************************************************/
/*  iPSC/860 (IPSC) dtime() for i860.                       */
/*  Provided by: Dan Yergeau, yergeau@gloworm.Stanford.EDU  */
/************************************************************/
#ifdef IPSC
extern double dclock();

double dtime()
{
	double q;

	q = dclock();
	
	return q;
}
#endif

/**************************************************/
/*  FORTRAN dtime() for Cray type systems.        */
/*  This is the preferred timer for Cray systems. */
/**************************************************/
#ifdef FORTRAN_SEC

fortran double second();

double dtime()
{
	double q;

	second(&q);
	
	return q;
}
#endif

/***********************************************************/
/*  UNICOS C dtime() for Cray UNICOS systems.  Don't use   */
/*  unless absolutely necessary as returned time includes  */
/*  'user+system' time.  Provided by: R. Mike Dority,      */
/*  dority@craysea.cray.com                                */
/***********************************************************/
#ifdef CTimer
#include <time.h>

double dtime()
{
	double    q;
	clock_t   clock(void);

		 q = (double)clock() / (double)CLOCKS_PER_SEC;

		 return q;
}
#endif

/********************************************/
/* Another UNIX timer using gettimeofday(). */
/* However, getrusage() is preferred.       */
/********************************************/
#ifdef GTODay
#include <sys/time.h>

struct timeval tnow;

double dtime()
{
	double q;

	gettimeofday(&tnow,NULL);
	q = (double)tnow.tv_sec + (double)tnow.tv_usec * 1.0e-6;

	return q;
}
#endif

/*****************************************************/
/*  Fujitsu UXP/M timer.                             */
/*  Provided by: Mathew Lim, ANUSF, M.Lim@anu.edu.au */
/*****************************************************/
#ifdef UXPM
#include <sys/types.h>
#include <sys/timesu.h>
struct tmsu rusage;

double dtime()
{
	double q;

	timesu(&rusage);

	q = (double)(rusage.tms_utime) * 1.0e-06;
	
	return q;
}
#endif

/**********************************************/
/*    Macintosh (MAC_TMgr) Think C dtime()    */
/*   requires Think C Language Extensions or  */
/*    #include <MacHeaders> in the prefix     */
/*  provided by Francis H Schiffer 3rd (fhs)  */
/*         skipschiffer@genie.geis.com        */
/**********************************************/
#ifdef MAC_TMgr
#include <Timer.h>
#include <stdlib.h>

static TMTask   mgrTimer;
static Boolean  mgrInited = false;
static double   mgrClock;

#define RMV_TIMER RmvTime( (QElemPtr)&mgrTimer )
#define MAX_TIME  1800000000L
/* MAX_TIME limits time between calls to */
/* dtime( ) to no more than 30 minutes   */
/* this limitation could be removed by   */
/* creating a completion routine to sum  */
/* 30 minute segments (fhs 1994 feb 9)   */

static void     Remove_timer( )
{
	RMV_TIMER;
	mgrInited = false;
}
double  dtime( )
{
	if( mgrInited ) {
		RMV_TIMER;
		mgrClock += (MAX_TIME + mgrTimer.tmCount)*1.0e-6;
	} else {
		if( _atexit( &Remove_timer ) == 0 ) mgrInited = true;
		mgrClock = 0.0;
	}
	if( mgrInited ) {
		mgrTimer.tmAddr = NULL;
		mgrTimer.tmCount = 0;
		mgrTimer.tmWakeUp = 0;
		mgrTimer.tmReserved = 0;
		InsTime( (QElemPtr)&mgrTimer );
		PrimeTime( (QElemPtr)&mgrTimer, -MAX_TIME );
	}
	return( mgrClock );
}
#endif

/***********************************************************/
/*  Parsytec GCel timer.                                   */
/*  Provided by: Georg Wambach, gw@informatik.uni-koeln.de */
/***********************************************************/
#ifdef PARIX
#include <sys/time.h>

double dtime()
{
   double q;

   q = (double) (TimeNowHigh()) / (double) CLK_TCK_HIGH;

   return q;
}
#endif

/************************************************/
/*  Sun Solaris POSIX dtime() routine           */
/*  Provided by: Case Larsen, CTLarsen.lbl.gov  */
/************************************************/
#ifdef POSIX
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/rusage.h>

#ifdef __hpux
#include <sys/syscall.h>
#endif

struct rusage rusage;

double dtime()
{
	double q;

	getrusage(RUSAGE_SELF,&rusage);

	q = (double)(rusage.ru_utime.tv_sec);
	q = q + (double)(rusage.ru_utime.tv_nsec) * 1.0e-09;
	
	return q;
}
#endif
