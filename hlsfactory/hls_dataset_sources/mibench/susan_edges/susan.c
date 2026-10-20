/**********************************************************************/
/*  SUSAN Version 2l by Stephen Smith                                 */
/*  Oxford Centre for Functional Magnetic Resonance Imaging of the    */
/*  Brain, Department of Clinical Neurology, Oxford University, UK    */
/*  (Previously at DRA Farnborough, UK.)                              */
/*  Email: steve@fmrib.ox.ac.uk                                       */
/*  See the original susan.c of MiBench automotive/susan for the      */
/*  full copyright and licence statement.                             */
/**********************************************************************/
/*
 * MiBench automotive/susan: setup_brightness_lut(), susan_edges(),
 * susan_thin() and edge_draw() for the default "-e" mode.
 *
 * HLS adaptation: static buffers instead of malloc, fixed image size, and
 * the unrolled pointer walks over the 37 pixel circular mask are loops
 * over the mask (the weights of the original statements are dx, dy for
 * the gradient and dx*dx, dy*dy, dx*dy for the symmetry measure).  The
 * thinning code is the original, with the pointer arithmetic turned into
 * indexing.  No file I/O.
 */
#include <math.h>
#include "susan.h"

#define BT 20            /* brightness threshold */
#define MAX_NO 2650      /* geometric threshold (max_no_edges) */
#define x_size SUSAN_X_SIZE
#define y_size SUSAN_Y_SIZE
#define NPIX (SUSAN_X_SIZE * SUSAN_Y_SIZE)

/* brightness look-up table: lut[k + 256] for k in [-256, 256], "form 6" */
static void setup_brightness_lut(uchar lut[513], int thresh)
{
    for (int k = -256; k < 257; k++) {
        float temp = ((float)k) / ((float)thresh);
        temp = temp * temp;
        temp = temp * temp * temp;
        temp = 100.0f * exp(-temp);
        lut[k + 256] = (uchar)temp;
    }
}

/* half width of each row of the 37 pixel circular mask, rows -3..+3 */
static const int half_width[7] = {1, 2, 3, 3, 3, 2, 1};

/* only one pass is needed as i,j are decremented if necessary to go
   back and do bits again */
static void susan_thin(const int r[NPIX], uchar mid[NPIX])
{
    int l[9], centre, nlinks, npieces,
        b01, b12, b21, b10,
        p1, p2, p3, p4,
        b00, b02, b20, b22,
        m, n, a = 0, b = 0, x, y, i, j;

    (void)nlinks;
    (void)npieces;
  for (i=4;i<SUSAN_Y_SIZE-4;i++)
    for (j=4;j<SUSAN_X_SIZE-4;j++)
      if (mid[i*SUSAN_X_SIZE+j]<8)
      {
        centre = r[i*SUSAN_X_SIZE+j];

        n = (mid[(i-1)*SUSAN_X_SIZE+j-1]<8) +
            (mid[(i-1)*SUSAN_X_SIZE+j]<8) +
            (mid[(i-1)*SUSAN_X_SIZE+j+1]<8) +
            (mid[(i)*SUSAN_X_SIZE+j-1]<8) +
            (mid[(i)*SUSAN_X_SIZE+j+1]<8) +
            (mid[(i+1)*SUSAN_X_SIZE+j-1]<8) +
            (mid[(i+1)*SUSAN_X_SIZE+j]<8) +
            (mid[(i+1)*SUSAN_X_SIZE+j+1]<8);

        if (n==0)
          mid[i*SUSAN_X_SIZE+j]=100;

        /* extension is only allowed a few times - the value of mid is used to control this */

        if ( (n==1) && (mid[i*SUSAN_X_SIZE+j]<6) )
        {
          /* find maximum neighbour weighted in direction opposite the
             neighbour already present. e.g.
             have: O O O  weight r by 0 2 3
                   X X O              0 0 4
                   O O O              0 2 3     */

          l[0]=r[(i-1)*SUSAN_X_SIZE+j-1]; l[1]=r[(i-1)*SUSAN_X_SIZE+j]; l[2]=r[(i-1)*SUSAN_X_SIZE+j+1];
          l[3]=r[(i  )*SUSAN_X_SIZE+j-1]; l[4]=0;                 l[5]=r[(i  )*SUSAN_X_SIZE+j+1];
          l[6]=r[(i+1)*SUSAN_X_SIZE+j-1]; l[7]=r[(i+1)*SUSAN_X_SIZE+j]; l[8]=r[(i+1)*SUSAN_X_SIZE+j+1];

          if (mid[(i-1)*SUSAN_X_SIZE+j-1]<8)        { l[0]=0; l[1]=0; l[3]=0; l[2]*=2; 
                                                l[6]*=2; l[5]*=3; l[7]*=3; l[8]*=4; }
          else { if (mid[(i-1)*SUSAN_X_SIZE+j]<8)   { l[1]=0; l[0]=0; l[2]=0; l[3]*=2; 
                                                l[5]*=2; l[6]*=3; l[8]*=3; l[7]*=4; }
          else { if (mid[(i-1)*SUSAN_X_SIZE+j+1]<8) { l[2]=0; l[1]=0; l[5]=0; l[0]*=2; 
                                                l[8]*=2; l[3]*=3; l[7]*=3; l[6]*=4; }
          else { if (mid[(i)*SUSAN_X_SIZE+j-1]<8)   { l[3]=0; l[0]=0; l[6]=0; l[1]*=2; 
                                                l[7]*=2; l[2]*=3; l[8]*=3; l[5]*=4; }
          else { if (mid[(i)*SUSAN_X_SIZE+j+1]<8)   { l[5]=0; l[2]=0; l[8]=0; l[1]*=2; 
                                                l[7]*=2; l[0]*=3; l[6]*=3; l[3]*=4; }
          else { if (mid[(i+1)*SUSAN_X_SIZE+j-1]<8) { l[6]=0; l[3]=0; l[7]=0; l[0]*=2; 
                                                l[8]*=2; l[1]*=3; l[5]*=3; l[2]*=4; }
          else { if (mid[(i+1)*SUSAN_X_SIZE+j]<8)   { l[7]=0; l[6]=0; l[8]=0; l[3]*=2; 
                                                l[5]*=2; l[0]*=3; l[2]*=3; l[1]*=4; }
          else { if (mid[(i+1)*SUSAN_X_SIZE+j+1]<8) { l[8]=0; l[5]=0; l[7]=0; l[6]*=2; 
                                                l[2]*=2; l[1]*=3; l[3]*=3; l[0]*=4; } }}}}}}}

          m=0;     /* find the highest point */
          for(y=0; y<3; y++)
            for(x=0; x<3; x++)
              if (l[y+y+y+x]>m) { m=l[y+y+y+x]; a=y; b=x; }

          if (m>0)
          {
            if (mid[i*SUSAN_X_SIZE+j]<4)
              mid[(i+a-1)*SUSAN_X_SIZE+j+b-1] = 4;
            else
              mid[(i+a-1)*SUSAN_X_SIZE+j+b-1] = mid[i*SUSAN_X_SIZE+j]+1;
            if ( (a+a+b) < 3 ) /* need to jump back in image */
	    {
              i+=a-1;
              j+=b-2;
              if (i<4) i=4;
              if (j<4) j=4;
	    }
	  }
        }

        if (n==2)
	{
          /* put in a bit here to straighten edges */
          b00 = mid[(i-1)*SUSAN_X_SIZE+j-1]<8; /* corners of 3x3 */
          b02 = mid[(i-1)*SUSAN_X_SIZE+j+1]<8;
	  b20 = mid[(i+1)*SUSAN_X_SIZE+j-1]<8;
          b22 = mid[(i+1)*SUSAN_X_SIZE+j+1]<8;
          if ( ((b00+b02+b20+b22)==2) && ((b00|b22)&(b02|b20)))
	  {  /* case: move a point back into line.
                e.g. X O X  CAN  become X X X
                     O X O              O O O
                     O O O              O O O    */
            if (b00) 
	    {
              if (b02) { x=0; y=-1; }
              else     { x=-1; y=0; }
	    }
            else
	    {
              if (b02) { x=1; y=0; }
              else     { x=0; y=1; }
	    }
            if (((float)r[(i+y)*SUSAN_X_SIZE+j+x]/(float)centre) > 0.7)
	    {
              if ( ( (x==0) && (mid[(i+(2*y))*SUSAN_X_SIZE+j]>7) && (mid[(i+(2*y))*SUSAN_X_SIZE+j-1]>7) && (mid[(i+(2*y))*SUSAN_X_SIZE+j+1]>7) ) ||
                   ( (y==0) && (mid[(i)*SUSAN_X_SIZE+j+(2*x)]>7) && (mid[(i+1)*SUSAN_X_SIZE+j+(2*x)]>7) && (mid[(i-1)*SUSAN_X_SIZE+j+(2*x)]>7) ) )
	      {
                mid[(i)*SUSAN_X_SIZE+j]=100;
                mid[(i+y)*SUSAN_X_SIZE+j+x]=3;  /* no jumping needed */
	      }
	    }
	  }
          else
          {
            b01 = mid[(i-1)*SUSAN_X_SIZE+j  ]<8;
            b12 = mid[(i  )*SUSAN_X_SIZE+j+1]<8;
            b21 = mid[(i+1)*SUSAN_X_SIZE+j  ]<8;
            b10 = mid[(i  )*SUSAN_X_SIZE+j-1]<8;

            if ( ((b01+b12+b21+b10)==2) && ((b10|b12)&(b01|b21)) &&
                 ((b01&((mid[(i-2)*SUSAN_X_SIZE+j-1]<8)|(mid[(i-2)*SUSAN_X_SIZE+j+1]<8)))|(b10&((mid[(i-1)*SUSAN_X_SIZE+j-2]<8)|(mid[(i+1)*SUSAN_X_SIZE+j-2]<8)))|
                (b12&((mid[(i-1)*SUSAN_X_SIZE+j+2]<8)|(mid[(i+1)*SUSAN_X_SIZE+j+2]<8)))|(b21&((mid[(i+2)*SUSAN_X_SIZE+j-1]<8)|(mid[(i+2)*SUSAN_X_SIZE+j+1]<8)))) )
	    { /* case; clears odd right angles.
                 e.g.; O O O  becomes O O O
                       X X O          X O O
                       O X O          O X O     */
              mid[(i)*SUSAN_X_SIZE+j]=100;
              i--;               /* jump back */
              j-=2;
              if (i<4) i=4;
              if (j<4) j=4;
	    }
	  }
	}

        if (n>2)
        {
          b01 = mid[(i-1)*SUSAN_X_SIZE+j  ]<8;
          b12 = mid[(i  )*SUSAN_X_SIZE+j+1]<8;
          b21 = mid[(i+1)*SUSAN_X_SIZE+j  ]<8;
          b10 = mid[(i  )*SUSAN_X_SIZE+j-1]<8;
          if((b01+b12+b21+b10)>1)
          {
            b00 = mid[(i-1)*SUSAN_X_SIZE+j-1]<8;
            b02 = mid[(i-1)*SUSAN_X_SIZE+j+1]<8;
	    b20 = mid[(i+1)*SUSAN_X_SIZE+j-1]<8;
	    b22 = mid[(i+1)*SUSAN_X_SIZE+j+1]<8;
            p1 = b00 | b01;
            p2 = b02 | b12;
            p3 = b22 | b21;
            p4 = b20 | b10;

            if( ((p1 + p2 + p3 + p4) - ((b01 & p2)+(b12 & p3)+(b21 & p4)+(b10 & p1))) < 2)
            {
              mid[(i)*SUSAN_X_SIZE+j]=100;
              i--;
              j-=2;
              if (i<4) i=4;
              if (j<4) j=4;
            }
          }
        }

      }
}

/* mark 3x3 white block around each edge point, then 1 black pixel at each edge point */
static void edge_draw(uchar in[NPIX], const uchar mid[NPIX])
{
    for (int i = 0; i < NPIX; i++) {
        if (mid[i] < 8) {
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                    if (!(dy == 0 && dx == 0))
                        in[i + dy * x_size + dx] = 255;
        }
    }
    for (int i = 0; i < NPIX; i++) {
        if (mid[i] < 8)
            in[i] = 0;
    }
}

int susan_edges(const uchar image[NPIX], uchar out[NPIX])
{
    static int r[NPIX];
    static uchar mid[NPIX];
    uchar bp[513];
    const uchar *in = image;
    const int max_no = MAX_NO;
    float z;
    int do_symmetry, i, j, m, n, a, b, x, y, w;
    int count = 0;

    setup_brightness_lut(bp, BT);

    for (i = 0; i < NPIX; i++) {
        r[i] = 0;
        mid[i] = 100; /* note not set to zero */
        out[i] = image[i];
    }

    /* edge response: USAN area of every pixel */
    for (i = 3; i < y_size - 3; i++)
        for (j = 3; j < x_size - 3; j++) {
            int centre = in[i * x_size + j];
            n = 100;
            for (int dy = -3; dy <= 3; dy++)
                for (int dx = -3; dx <= 3; dx++)
                    if (dx >= -half_width[dy + 3] && dx <= half_width[dy + 3] && !(dy == 0 && dx == 0))
                        n += bp[centre - in[(i + dy) * x_size + j + dx] + 256];

            if (n <= max_no)
                r[i * x_size + j] = max_no - n;
        }

    /* direction of each edge and non-maximum suppression */
    for (i = 4; i < y_size - 4; i++)
        for (j = 4; j < x_size - 4; j++) {
            if (r[i * x_size + j] > 0) {
                int centre = in[i * x_size + j];

                m = r[i * x_size + j];
                n = max_no - m;

                if (n > 600) {
                    x = 0;
                    y = 0;
                    for (int dy = -3; dy <= 3; dy++)
                        for (int dx = -3; dx <= 3; dx++)
                            if (dx >= -half_width[dy + 3] && dx <= half_width[dy + 3] && !(dy == 0 && dx == 0)) {
                                int c = bp[centre - in[(i + dy) * x_size + j + dx] + 256];
                                x += c * dx;
                                y += c * dy;
                            }

                    z = sqrt((float)((x * x) + (y * y)));
                    if (z > (0.9 * (float)n)) { /* 0.5 */
                        do_symmetry = 0;
                        if (x == 0)
                            z = 1000000.0;
                        else
                            z = ((float)y) / ((float)x);
                        if (z < 0) {
                            z = -z;
                            w = -1;
                        } else
                            w = 1;
                        if (z < 0.5) { /* vert_edge */
                            a = 0;
                            b = 1;
                        } else {
                            if (z > 2.0) { /* hor_edge */
                                a = 1;
                                b = 0;
                            } else { /* diag_edge */
                                if (w > 0) {
                                    a = 1;
                                    b = 1;
                                } else {
                                    a = -1;
                                    b = 1;
                                }
                            }
                        }
                        if ((m > r[(i + a) * x_size + j + b]) && (m >= r[(i - a) * x_size + j - b]) &&
                            (m > r[(i + (2 * a)) * x_size + j + (2 * b)]) &&
                            (m >= r[(i - (2 * a)) * x_size + j - (2 * b)]))
                            mid[i * x_size + j] = 1;
                    } else
                        do_symmetry = 1;
                } else
                    do_symmetry = 1;

                if (do_symmetry == 1) {
                    x = 0;
                    y = 0;
                    w = 0;
                    for (int dy = -3; dy <= 3; dy++)
                        for (int dx = -3; dx <= 3; dx++)
                            if (dx >= -half_width[dy + 3] && dx <= half_width[dy + 3] && !(dy == 0 && dx == 0)) {
                                int c = bp[centre - in[(i + dy) * x_size + j + dx] + 256];
                                x += c * dx * dx;
                                y += c * dy * dy;
                                w += c * dx * dy;
                            }

                    if (y == 0)
                        z = 1000000.0;
                    else
                        z = ((float)x) / ((float)y);
                    if (z < 0.5) { /* vertical */
                        a = 0;
                        b = 1;
                    } else {
                        if (z > 2.0) { /* horizontal */
                            a = 1;
                            b = 0;
                        } else { /* diagonal */
                            if (w > 0) {
                                a = -1;
                                b = 1;
                            } else {
                                a = 1;
                                b = 1;
                            }
                        }
                    }
                    if ((m > r[(i + a) * x_size + j + b]) && (m >= r[(i - a) * x_size + j - b]) &&
                        (m > r[(i + (2 * a)) * x_size + j + (2 * b)]) &&
                        (m >= r[(i - (2 * a)) * x_size + j - (2 * b)]))
                        mid[i * x_size + j] = 2;
                }
            }
        }

    susan_thin(r, mid);
    edge_draw(out, mid);

    for (i = 0; i < NPIX; i++)
        count += (mid[i] < 8);
    return count;
}
