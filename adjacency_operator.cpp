#include <vector>
#include "adjacency_operator.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class AdjacencyOperator::GradientThresholdImpl
{
    int threshold;
    std::vector<cv::Mat> channels;
    cv::Mat gradX, gradY, gradNorm;
public:
    GradientThresholdImpl( int threshold ) : threshold( threshold ) { }
    void SetThreshold( int newThreshold ) { threshold = newThreshold; }
    void Compute( const cv::Mat & image, cv::Mat & verticalAdjacency, cv::Mat & horizontalAdjacency )
    {
        cv::split( image, channels );
        //gradX.create( cv::Size(image.cols, image.rows) , CV_32SC1 );
        //gradY.create( cv::Size(image.cols, image.rows) , CV_32SC1 );
        gradNorm.create( image.rows, image.cols, CV_32F );
        gradNorm = 0;
        for ( auto & channel : channels )
        {
            cv::Scharr( channel, gradX, CV_32F, 1, 0, 1.0 / 32.0 );
            cv::Scharr( channel, gradY, CV_32F, 0, 1, 1.0 / 32.0 );
            cv::multiply( gradX, gradX, gradX );
            cv::multiply( gradY, gradY, gradY );
            gradX += gradY;
            gradX.forEach<float>( [](float & value, const int * /*position */ ){ value = std::sqrt(value); } );
            gradNorm += gradX;
        }

        verticalAdjacency.create( image.rows, image.cols, CV_8U );
        horizontalAdjacency.create( image.rows, image.cols, CV_8U);
        verticalAdjacency = 0;
        horizontalAdjacency = 0;
        for (int row = 1; row < image.rows; ++row )
        {
            uint8_t * vertAdjVal = verticalAdjacency.ptr( row );
            uint8_t * horizAdjVal = horizontalAdjacency.ptr( row );
            const float * gradVal = gradNorm.ptr<float>( row );
            if ( gradVal[0 - gradNorm.step1()] < threshold )
                vertAdjVal[0] = 1;

            for ( int col = 1; col < image.cols; ++col )
            {
                if ( gradVal[col - 1] < threshold )
                    horizAdjVal[col] = 1;
                if ( gradVal[col - gradNorm.step1()] < threshold )
                    vertAdjVal[col] = 1;
            }
        }
    }
};

class AdjacencyOperator::LocalOtsuSegmentationImpl
{
    int neighborhoodSize;
public:
    LocalOtsuSegmentationImpl( int neighborhoodSize ) : neighborhoodSize( neighborhoodSize ) {}
    void Compute( const cv::Mat & image, cv::Mat & verticalAdjacency, cv::Mat & horizontalAdjacency ) const
    {
    }
};

void AdjacencyOperator::operator()( const cv::Mat & image, cv::Mat & verticalAdjacency, cv::Mat & horizontalAdjacency )
{
    if ( algorithm == GradientThreshold )
    {
        if ( gradientThresholdImpl == nullptr )
            gradientThresholdImpl = std::make_shared<GradientThresholdImpl>( gradientThreshold );
        gradientThresholdImpl->Compute( image, verticalAdjacency, horizontalAdjacency );
    }
    else if ( algorithm == LocalOtsuSegmentation )
    {
        if ( localOtsuSegmentationImpl == nullptr )
            localOtsuSegmentationImpl = std::make_shared<LocalOtsuSegmentationImpl>( neighborhoodSize );
        localOtsuSegmentationImpl->Compute( image, verticalAdjacency, horizontalAdjacency );
    }
    else
    {
        throw std::logic_error( "Not supported algorithm" );
    }
}

void AdjacencyOperator::SetAlgorithm( Algorithm newAlgorithm )
{
    if ( algorithm != GradientThreshold && algorithm != LocalOtsuSegmentation)
        throw std::logic_error( "Not supported algorithm" );
    algorithm = newAlgorithm;
}
