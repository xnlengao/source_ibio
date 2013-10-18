/*************************************************************************** 
 *	Module Name:	WULine
 *
 *	Abstract:	反走样直线技术
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-29 09:58:26
 ***************************************************************************/
#include "IncludeFiles.h"
#include "assert.h"



/*
	定点运算的函数
*/
//返回整数部分
static int b_trunc(double dValue)
{
	double i;
	double f;
	
	f = modf(dValue, &i);
	
	return (int)i;
}
//返回小数部分
static double b_frac(double dValue)
{
	double i;
	double f;
	
	f = modf(dValue, &i);
	
	return (double)f;
}
//翻转小数
static double b_invfrac(double dValue)
{
	double i;
	double f;
	
	f = modf(dValue, &i);
	f = 1.0 - f;
	
	return (double)f;
}

int DrawWuLine(HDC hdc, int X0, int Y0, int X1, int Y1, gal_pixel clrLine)
{
	/* Make sure the line runs top to bottom */
	if (Y0 > Y1)	{
		int Temp = Y0; Y0 = Y1; Y1 = Temp;
		Temp = X0; X0 = X1; X1 = Temp;
	}
    
   	 /* Draw the initial pixel, which is always exactly intersected by
	the line and so needs no weighting */
	SetPixel(hdc, X0,Y0,clrLine);
	
	int XDir, DeltaX = X1 - X0;
	if( DeltaX >= 0 ){
		XDir = 1;
	}
	else{
		XDir   = -1;
		DeltaX = 0 - DeltaX; /* make DeltaX positive */
	}
	
	/* Special-case horizontal, vertical, and diagonal lines, which
	require no weighting because they go right through the center of
	every pixel */
	int DeltaY = Y1 - Y0;
	if (DeltaY == 0){
		/* Horizontal line */
		while (DeltaX-- != 0){
			X0 += XDir;
			SetPixel(hdc,X0,Y0, clrLine );
		}
		return;
	}
	if (DeltaX == 0){
		/* Vertical line */
		do{
			Y0++;
			SetPixel(hdc, X0, Y0, clrLine );
		} while (--DeltaY != 0);
		return;
	}

	if (DeltaX == DeltaY){
		/* Diagonal line */
		do{
			X0 += XDir;
			Y0++;
			SetPixel(hdc, X0, Y0, clrLine );
		} while (--DeltaY != 0);
		return 0;
	}
	
	unsigned short ErrorAdj;
	unsigned short ErrorAccTemp, Weighting;

	/* Line is not horizontal, diagonal, or vertical */
	unsigned short ErrorAcc = 0;  /* initialize the line error accumulator to 0 */

// 	BYTE rl = GetRValue( clrLine );
// 	BYTE gl = GetGValue( clrLine );
// 	BYTE bl = GetBValue( clrLine );
	BYTE rl, gl, bl;
	Pixel2RGB(hdc, clrLine, &rl, &gl, &bl);
	double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114;
	
	/* Is this an X-major or Y-major line? */
	if (DeltaY > DeltaX){
    		/* Y-major line; calculate 16-bit fixed-point fractional part of a
		pixel that X advances each time Y advances 1 pixel, truncating the
		result so that we won't overrun the endpoint along the X axis */
 		ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
		/* Draw all pixels other than the first and last */
		while (--DeltaY) {
			ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
			ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
			if (ErrorAcc <= ErrorAccTemp) {
				/* The error accumulator turned over, so advance the X coord */
				X0 += XDir;
			}
			Y0++; /* Y-major, so always advance Y */
                	 /* The IntensityBits most significant bits of ErrorAcc give us the
			intensity weighting for this pixel, and the complement of the
			weighting for the paired pixel */
// 			Weighting = ErrorAcc >> 8;
			Weighting = ErrorAcc >>7;
// 			assert( Weighting < 256 );
// 			assert( ( Weighting ^ 255 ) < 256 );
            
 			gal_pixel clrBackGround = GetPixel(hdc, X0, Y0 );
// 			BYTE rb = GetRValue( clrBackGround );
// 			BYTE gb = GetGValue( clrBackGround );
// 			BYTE bb = GetBValue( clrBackGround );
			BYTE rb, gb, bb;
			Pixel2RGB(hdc, clrBackGround, &rb, &gb, &bb);
			double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
            
			BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
			BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
			BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
// 			SetPixelRGB(hdc, X0, Y0, rr, gr, br);
			SetPixel(hdc, X0, Y0, RGB2Pixel(hdc, rr, gr, br));

// 			printf("Debug 1\n;");
			
			clrBackGround = GetPixel(hdc, X0 + XDir, Y0 );
// 			rb = GetRValue( clrBackGround );
// 			gb = GetGValue( clrBackGround );
// 			bb = GetBValue( clrBackGround );
			Pixel2RGB(hdc, clrBackGround, &rb, &gb, &bb);
			grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
            
			rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
			gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
			br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
// 			SetPixelRGB(hdc, X0 + XDir, Y0, rr, gr, br );
			SetPixel(hdc, X0, Y0, RGB2Pixel(hdc, rr, gr, br));
		}
	        /* Draw the final pixel, which is always exactly intersected by the line
		and so needs no weighting */
		SetPixel(hdc, X1, Y1, clrLine );
		return 0;
		
	}
	
	/* It's an X-major line; calculate 16-bit fixed-point fractional part of a
	pixel that Y advances each time X advances 1 pixel, truncating the
	result to avoid overrunning the endpoint along the X axis */
 	ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
	/* Draw all pixels other than the first and last */
	while (--DeltaX) {
		ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
		ErrorAcc += ErrorAdj;          /* calculate error for next pixel */
		if (ErrorAcc <= ErrorAccTemp) {
			/* The error accumulator turned over, so advance the Y coord */
			Y0++;
		}
		X0 += XDir; /* X-major, so always advance X */                           
                 /* The IntensityBits most significant bits of ErrorAcc give us the
		intensity weighting for this pixel, and the complement of the
		weighting for the paired pixel */
//  		Weighting = ErrorAcc >> 8;
		Weighting = ErrorAcc >> 7;
//  		assert( Weighting < 256 );
//  		assert( ( Weighting ^ 255 ) < 256 );

 		gal_pixel clrBackGround = GetPixel(hdc, X0, Y0 );
// 		BYTE rb = GetRValue( clrBackGround );
// 		BYTE gb = GetGValue( clrBackGround );
// 		BYTE bb = GetBValue( clrBackGround );
		BYTE rb, gb, bb;
		Pixel2RGB(hdc, clrBackGround, &rb, &gb, &bb);
		
		double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

		BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
		BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
		BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
//    		SetPixelRGB(hdc, X0, Y0, rr, gr, br);
		SetPixel(hdc, X0, Y0, RGB2Pixel(hdc, rr, gr, br));
		
//  		printf("Debug 2 Point(%d, %d) r: %d, g:%d, b:%d\n.", X0, Y0, rb, gb, bb);

		clrBackGround = GetPixel(hdc, X0, Y0 + 1 );

// 		rb = GetRValue( clrBackGround );
// 		gb = GetGValue( clrBackGround );
// 		bb = GetBValue( clrBackGround );
		Pixel2RGB(hdc, clrBackGround, &rb, &gb, &bb);
		
 		grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
        
		rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
		gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
		br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
        
//   		SetPixelRGB(hdc, X0, Y0 + 1, rr, gr, br); 
		SetPixel(hdc, X0, Y0, RGB2Pixel(hdc, rr, gr, br));
	}
	
	/* Draw the final pixel, which is always exactly intersected by the line
	and so needs no weighting */
	SetPixel(hdc, X1, Y1, clrLine );
	
}

//Generates spokes. Animates them using phase.
static void GenerateSpokes (HDC hdc, float phase)
{                                               
	short x, y, x2, y2;
	float theta;
	
	
	SetPenColor(hdc, COLOR_green);
	for (theta= phase; theta< 360+phase; theta += 10 )
	{
// 		x = (short)(100.0*cos(theta*3.14/180.0)+355.0);
// 		y = (short)(-100.0*sin(theta*3.14/180.0)+155.0);
		x = (short)(50.0*cos(theta*3.14/180.0)+355.0);
		y = (short)(-50.0*sin(theta*3.14/180.0)+100.0);
		
		x2 = (short)(20.0*cos(theta*3.14/180.0)+355.0);
		y2 = (short)(-20.0*sin(theta*3.14/180.0)+100.0);
		
   		DrawWuLine (hdc,x, y, x2, y2, RGB2Pixel(hdc,0,128,0));
//   		DrawWuLine (hdc,x, y, x2, y2, COLOR_green);
		
		MoveTo (hdc, x2-240,y2);
		LineTo (hdc, x-240,y);
	}
}


/**
 * Bresenham Line Algorithm
 * @param dashedMask 设置线型的虚线的间隔，为0则画实线。
 * @param lineWidth 设置线宽。
 * @param x1 
 * @param y1
 * @param x2
 * @param y2
 */
 void bresenhamLine(HDC hdc, int dashedMask, int lineWidth, int x1, int y1, int x2, int y2)
{
	int  x, y;
	int  dx, dy;
	int  incx, incy;
	int  balance;
	int i=0;
	int j=0;
	
	if (x2 >= x1){
		dx = x2 - x1;
		incx = 1;
	}
	else{
		dx = x1 - x2;
		incx = -1;
	}
	
	if (y2 >= y1){
		dy = y2 - y1;
		incy = 1;
	}
	else{
		dy = y1 - y2;
		incy = -1;
	}

	x = x1;
	y = y1;

	if (dx >= dy){
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;

		while (x != x2){
			if ( (i & dashedMask)==0 ){
				for(j=0; j<=lineWidth; j++){
					SetPixel(hdc, x, y+j,GetPenColor(hdc));
				}
//  				Rectangle(hdc, x, y,x+lineWidth, y+lineWidth);
//  				Circle(hdc, x, y,lineWidth);
			}
				
			if (balance >= 0){
				y += incy;
				balance -= dx;
			}
			balance += dy;
			x += incx;
			i ++;
		}
		if ( (i & dashedMask)==0 ){
			for(j=0; j<=lineWidth; j++){
				SetPixel(hdc, x, y+j,GetPenColor(hdc));
			}
//  			Rectangle(hdc, x, y,x+lineWidth, y+lineWidth);
//  			Circle(hdc, x, y,lineWidth);
		}
	}
	else{
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (y != y2){
			if ( (i & dashedMask)==0 ){
				for(j=0; j<=lineWidth; j++){
					SetPixel(hdc, x, y+j,GetPenColor(hdc));
				}
//  				Rectangle(hdc, x, y,x+lineWidth, y+lineWidth);
//  				Circle(hdc, x, y,lineWidth);
			}
			if (balance >= 0){
				x += incx;
				balance -= dy;
			}
			balance += dx;
			y += incy;
			i ++;
		}
		if ( (i & dashedMask)==0 ){
			for(j=0; j<=lineWidth; j++){
				SetPixel(hdc, x, y+j,GetPenColor(hdc));
			}
//  			Rectangle(hdc, x, y,x+lineWidth, y+lineWidth);
//  			Circle(hdc, x, y,lineWidth);
		}
	}
}

//---------------- interface ----------
int WuLineTest()
{
	double dTestValue = 10.326;
	int iValue = 0;
	double dValue = 0.0;
	double invdValue = 0.0;
	
	iValue = b_trunc(dTestValue);
	dValue = b_frac(dTestValue);
	invdValue = b_invfrac(dTestValue);
	
	printf("valis is %f, iValue is %d, dValue is %f. invdValue is %f\n", dTestValue, iValue, dValue, invdValue);
	
   	GenerateSpokes (HDC_SCREEN, 0.f);

	bresenhamLine(HDC_SCREEN, 5, 0, 90, 250, 350, 250);
	
//   	DrawWuLine (,300, 200, 310, 206, COLOR_green);

	return 0;
}

