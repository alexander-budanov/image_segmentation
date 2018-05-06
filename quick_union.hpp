#pragma once
#include <vector>

class QuickUnion
{
    std::vector<int> labels;
    std::vector<int> sizes;
public:
    QuickUnion( int size ) :
        labels( size ),
        sizes( size, 1 ) // vector of length == size filled with 1's
    {
        for ( int i = 0; i < size; ++i )
            labels[i] = i;
    }

    void Join( int n, int m ) // states that n and m belong to the same set
    {
        int label_n = Label( n );
        int label_m = Label( m );
        if ( label_n != label_m )
        {
            if ( sizes[label_n] < sizes[label_m] ) // merge smaller set into larger set
            {
                labels[label_n] = label_m;
                sizes[label_m] += sizes[label_n];
            }
            else
            {
                labels[label_m] = label_n;
                sizes[label_n] += sizes[label_m];
            }
        }
    }

    int Label( int n ) // returns label of the set containing n
    {
        while ( n != labels[n] )
        {
            labels[n] = labels[labels[n]];
            n = labels[n];
        }
        return n;
    }

    int Size( int n ) //  returns size of the set containing n
    {
        return sizes[Label( n )];
    }

    void Reset( int size )
    {
        labels.resize( size );
        sizes.resize( size );
        for ( int i = 0; i < size; ++i )
        {
            sizes[i] = 1;
            labels[i] = i;
        }
    }
};
