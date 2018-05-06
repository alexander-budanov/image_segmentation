#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "adjacency_operator.h"
#include "regions.h"

using namespace std;

const char * getArg( const char * key, int argc, const char * const argv[] )
{
    for ( int i = 1; i < argc - 1; ++i )
    {
        if ( strcmp( key, argv[i] ) == 0 ) 
            return argv[i + 1];
    }
    return nullptr;
}

int main( int argc, char ** argv )
{
    const char * inputPath = getArg("-i", argc, argv );
    if ( inputPath == nullptr )
    {
        cout << "No input specified. Use -i [input image path] to specify input image." << endl;
        cv::waitKey( 0 );
        return 0;
    }

    cv::Mat image;
    image = cv::imread( inputPath, CV_LOAD_IMAGE_COLOR );
    if ( image.empty() )
    {
        cout << "Failed to load image from " << inputPath << endl;
        cv::waitKey( 0 );
        return 0;
    }

    cv::imshow( "Original image", image );
    cv::namedWindow( "Regions image" );
    int gradiendThreshold = 30;
    cv::createTrackbar( "Gradient threshold", "Regions image", &gradiendThreshold, 255 );
    int prevGradientThreshold = 10;
    AdjacencyOperator adjOp( AdjacencyOperator::GradientThreshold );
    cv::Mat verticalAdj, horizontalAdj;

    while ( cv::waitKey( 200 ) != 27 )
    {
        if ( prevGradientThreshold != gradiendThreshold )
        {
            prevGradientThreshold = gradiendThreshold;
            adjOp.SetGradientTreshold( gradiendThreshold );
            adjOp( image, verticalAdj, horizontalAdj );
            cv::Mat regions = MakeRegionsMap( verticalAdj, horizontalAdj, gradiendThreshold );
            double min, max;
            cv::minMaxIdx( regions, &min, &max );
            cv::Mat coloredRegions;
            regions.convertTo( coloredRegions, CV_8UC1, 255 / ( max - min ), -min ); 
            cv::applyColorMap( coloredRegions, coloredRegions, cv::COLORMAP_RAINBOW);
            cv::imshow( "Regions image", coloredRegions );
            cv::setTrackbarPos( "Gradient threshold", "Regions image", gradiendThreshold );
        }
    }
    return 0;
}