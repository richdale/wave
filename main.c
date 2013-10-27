#include <stdio.h>
#include <math.h>
#include "cv.h"
#include "highgui.h"




static void centroid( IplImage *gsImg, int *x, int *y )
{
	/* calculates the centroid of an 8-bit grey scale img */
	int i, j;			    
	long xsum, ysum, mass;
	xsum = ysum = mass = 0;
	for( i=0; i<gsImg->width; i++ )
	{
		for( j=0; j<gsImg->height; j++ )
		{
			xsum += i*(gsImg->imageData[ i + j*(gsImg->widthStep) ] );      
			ysum += j*(gsImg->imageData[ i + j*(gsImg->widthStep) ] );      
			mass +=   (gsImg->imageData[ i + j*(gsImg->widthStep) ] );      
		}
	}
	*x = xsum/mass;
	*y = ysum/mass;
																								    
	//printf( "Centroid: (%d, %d)\n", *x, *y );

}





int main( int argc, char *argv[] )
{


	int i, j, k;
	int h, w, n, s;
	int camNo;

	CvCapture *webcam;

	//for( i=0; i<4; i++ )
	//{
	//	webcam = cvCaptureFromCAM( i );
	//	if( webcam )
	//	{
	//	printf( "Found camera at index %d\n", i );
	//	camNo = i;
	//	break;
	//	}
	//}

	webcam = cvCaptureFromCAM( atoi(argv[1]) );

	IplImage *f;



	cvNamedWindow( "webcam", CV_WINDOW_AUTOSIZE );

	

	//printf( "CV_WINDOW_AUTOSIZE = %d\n", CV_WINDOW_AUTOSIZE );

	//h = f->height;
	//w = f->width;
	//n = f->nChannels;
	//s = f->widthStep;


	//printf( "h=%d\nw=%d\n", h, w );

	




	
	//IplImage *imgBin = cvCreateImage( cvGetSize(f), 8, 1 );



	//printf( "about to start main loop\n" );
		
	/* MAIN LOOP */
	while( webcam )
	{
		printf( "attempt to grab a frame from camera index %d\n", i );
		f = cvQueryFrame( webcam );
		cvShowImage( "webcam", f );
		cvWaitKey(20);
		
	}
}
