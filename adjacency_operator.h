#pragma once
#include <memory>

namespace cv
{
    class Mat;
}

class AdjacencyOperator
{
public:
    enum Algorithm { GradientThreshold, LocalOtsuSegmentation };
    AdjacencyOperator( Algorithm algorithm ) { SetAlgorithm( algorithm ); }
    // computes adjacency between neighbor pixels of the input
    // output is 2 binary images:
    // verticalAdjacency.at( row, col ) == 1 iff ( row, col ) and ( row - 1, col) are adjacent pixels on the input image
    // horizontalAdjacency.at( row, col ) == 1 iff ( row, col ) and ( row, col - 1) are adjacent pixels on the input image
    void operator()( const cv::Mat & image, cv::Mat & verticalAdjacency, cv::Mat & horizontalAdjacency );
    void SetAlgorithm( Algorithm algorithm );
    void SetOtsuNeightborhoodSize( int size ) { neighborhoodSize = size; }
    void SetGradientTreshold( int threshold ) { gradientThreshold = threshold; }
private:
    class GradientThresholdImpl;
    class LocalOtsuSegmentationImpl; // TO DO: implement this algorithm
    Algorithm algorithm = Algorithm::GradientThreshold;
    int neighborhoodSize = 7;
    int gradientThreshold = 42;
    std::shared_ptr<GradientThresholdImpl> gradientThresholdImpl = nullptr;
    std::shared_ptr<LocalOtsuSegmentationImpl> localOtsuSegmentationImpl = nullptr;
};
