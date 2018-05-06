#include <unordered_map>
#include "regions.h"
#include "quick_union.hpp"

cv::Mat MakeRegionsMap( const cv::Mat & verticalAdjacency, const cv::Mat & horizontalAdjacency, int minSize )
{
    if ( verticalAdjacency.empty() || horizontalAdjacency.empty() ||
         verticalAdjacency.size() != horizontalAdjacency.size() ||
         verticalAdjacency.type() != CV_8U || horizontalAdjacency.type() != CV_8U )
    {
        throw std::logic_error( "Invalid argument" );
    }

    cv::Size size = verticalAdjacency.size();
    QuickUnion quickUnion( size.width * size.height ); 
    
    for (int row = 1; row < size.height; ++row )
    {
        const uint8_t * vertAdjVal = verticalAdjacency.ptr( row );
        const uint8_t * horizAdjVal = horizontalAdjacency.ptr( row );
        int rowIndex = row * size.width;
        if ( vertAdjVal[0] == 1 ) 
            quickUnion.Join( rowIndex, rowIndex - size.width );

        for ( int col = 1; col < size.width; ++col )
        {
            if ( vertAdjVal[col] == 1 ) 
                quickUnion.Join( rowIndex + col, rowIndex + col - size.width );
            if ( horizAdjVal[col] == 1 )
                quickUnion.Join( rowIndex + col, rowIndex + col - 1 );
        }
    }

    cv::Mat regionsMap( size, CV_32S );
    regionsMap = -1;
    std::unordered_map<int32_t, int32_t> labelsMap;
    int32_t prevLabel = -1;
    int32_t mappedLabel = -1;
    int32_t labelsCounter = 0;
    for (int row = 0; row < size.height; ++row )
    {
        int32_t * value = regionsMap.ptr<int32_t>( row );
        for ( int col = 0; col < size.width; ++col )
        {
            int32_t label = quickUnion.Label( row * size.width + col );
            if ( prevLabel != label )
            {
                auto it = labelsMap.find( label );
                if ( it != labelsMap.end() )
                {
                    mappedLabel = it->second;
                }
                else
                {
                    if ( quickUnion.Size( row * size.width + col ) >= minSize )
                        mappedLabel = labelsCounter++;
                    labelsMap.emplace( label, mappedLabel );
                }
                prevLabel = label;
            }
            value[col] = mappedLabel; 
        }
    }

    return regionsMap;
}
