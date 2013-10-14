#include <stdio.h>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "rs232.h"
#include "blob.h"



#define MDMC_BAUD 115200
#define MDMC_PORT 16       

#define M1_IDLE 0x40
#define M1_FWD  0x50
#define M1_REV  0x60
#define M1_BRK  0x70
#define M2_IDLE 0x80
#define M2_FWD  0x90
#define M2_REV  0xA0
#define M2_BRK  0xB0

#define DBAND 5


static void centroid( IplImage *gsImg, int *x, int *y ){
	/* calculates the centroid of an 8-bit grey scale img */
	int i, j;			    
	long xsum, ysum, mass;
	xsum = ysum = mass = 0;
	for( i=0; i<gsImg->width; i++ ){
		for( j=0; j<gsImg->height; j++ ){
			xsum += i*(gsImg->imageData[ i + j*(gsImg->widthStep) ] );      
			ysum += j*(gsImg->imageData[ i + j*(gsImg->widthStep) ] );      
			mass +=   (gsImg->imageData[ i + j*(gsImg->widthStep) ] );      
		}
	}
	*x = xsum/mass;
	*y = ysum/mass;
																								    
	//printf( "Centroid: (%d, %d)\n", *x, *y );

}





int main( int argc, char *argv[] ){


	int i, j, k;
	int h, w, n, s;
	unsigned char b, g, r;
	char rx_buffer[100];


	CvCapture *webcam;

	webcam = cvCaptureFromCAM( 0 );

	IplImage *f;

	/* grab the first few frames for good measure */
	for( i=0; i<5; i++ ){
		f = cvQueryFrame( webcam );
	}



	cvNamedWindow( "webcam", CV_WINDOW_AUTOSIZE );

	h = f->height;
	w = f->width;
	n = f->nChannels;
	s = f->widthStep;


	if( OpenComport( MDMC_PORT, MDMC_BAUD ) ){
		printf( "Failed to open comport \n" );
		exit(1);
	} else {
		printf( "Comport opened to MDMC\n" );
	}
	
	SendByte( MDMC_PORT, 'Q' );
	SendByte( MDMC_PORT, 'R' );


	int x0 = w/2;
	int y0 = h/2;


	printf( "(x0,y0) = (%d,%d)\n", x0, y0 );
	
	IplImage *imgBin = cvCreateImage( cvGetSize(f), 8, 1 );
	int valMin, magMin; 
	int xcm, ycm, xerr, yerr;
	int b0, g0, r0;
	float val, mag;	
	unsigned char xpwm, ypwm;

	valMin = 198;
	magMin = 141;
	b0 = 0;
	g0 = 0;
	r0 = 255;	
		
	cvCreateTrackbar( "val", "webcam", &valMin, 255, NULL );
	cvCreateTrackbar( "magmin", "webcam", &magMin, 255, NULL );
	cvCreateTrackbar( "r0", "webcam", &r0, 255, NULL );
	cvCreateTrackbar( "g0", "webcam", &g0, 255, NULL );
	cvCreateTrackbar( "b0", "webcam", &b0, 255, NULL );

	
	char key, running;
	running = 1;
	int kpx, kpy;
	
	kpx = 1;
	kpy = 2;   // EL is slower	


	printf( "about to start main loop\n" );
		
	/* MAIN LOOP */
	for(;;){
		f = cvQueryFrame( webcam );


		for( i=0; i<w; i++ ){
			for( j=0; j<h; j++ ){
				b = f->imageData[ i*n +j*s + 0 ];
				g = f->imageData[ i*n +j*s + 1 ];
				r = f->imageData[ i*n +j*s + 2 ];
				mag = sqrt( b*b + g*g + r*r );	
				val = ( b*b0 + g*g0 + r*r0 )/mag;
				

				if( (val > valMin)  && (mag > magMin) ){
					imgBin->imageData[ i + j*(imgBin->widthStep) ] = 255;
				} else {
					imgBin->imageData[ i + j*(imgBin->widthStep) ] = 0;
				}
			}
		}
		
		centroid( imgBin, &xcm, &ycm );		
		

		if( running ){
			xerr = xcm - x0;
			yerr = ycm - y0;
			printf( "xerr = %d, yerr = %d\n", xerr, yerr );			

			if( abs(xerr) < DBAND ){
				xpwm = 0;
				SendByte( MDMC_PORT, M1_IDLE );
				SendByte( MDMC_PORT, xpwm );
			} else if( xerr > 0 ){
				xpwm = ( xerr*kpx < 255 ) ? xerr*kpx : 255;
				SendByte( MDMC_PORT, M1_REV );
				SendByte( MDMC_PORT, xpwm );
			} else {
				xpwm = ( -1*xerr*kpx < 255 ) ? -1*xerr*kpx : 255;
				SendByte( MDMC_PORT, M1_FWD );
				SendByte( MDMC_PORT, xpwm );
			}
			if( abs(yerr) < DBAND ){
				ypwm = 0;
				SendByte( MDMC_PORT, M2_IDLE );
				SendByte( MDMC_PORT, ypwm );
			} else if( yerr > 0 ){
				ypwm =  ( yerr*kpy < 255 ) ? yerr*kpy : 255;
				SendByte( MDMC_PORT, M2_REV );
				SendByte( MDMC_PORT, ypwm );
			} else {
				ypwm = ( -1*yerr*kpy < 255 ) ? -1*yerr*kpy : 255;
				SendByte( MDMC_PORT, M2_FWD );
				SendByte( MDMC_PORT, ypwm );
			}
		}


		//cvLine( imgBin, cvPoint(xcm-15,ycm), cvPoint(xcm+15,ycm), cvScalar(0,0,0,0), 1, 8, 0);
		//cvLine( imgBin, cvPoint(xcm,ycm-15), cvPoint(xcm,ycm+15), cvScalar(0,0,0,0), 1, 8, 0);

		cvShowImage( "webcam", imgBin );



		key = cvWaitKey(20);
		if( key != -1 ){
			running = 1;
		}
	}
}
