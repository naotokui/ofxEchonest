//
//  ENAnalysisResult.h
//  echonest
//
//  Created by Nao Tokui on 11/13/14.
//  Copyright (c) 2014 Nao Tokui <www.naotokui.net>. All rights reserved.
//

#ifndef __echonest__ENAnalysisResult__
#define __echonest__ENAnalysisResult__

#include <stdio.h>
#include "ofxJSONElement.h"

// Echonest gives back 12 coefficient values to represent a given timbre
// --------------------------
// "timbre is the quality of a musical note or sound that distinguishes different types of musical instruments, or voices.
// It is a complex notion also referred to as sound color, texture, or tone quality, and is derived from the shape of a segment’s spectro-temporal surface,
// independently of pitch and loudness. The Echo Nest Analyzer’s timbre feature is a vector that includes 12 unbounded values roughly centered around 0.
// Those values are high level abstractions of the spectral surface, ordered by degree of importance. For completeness however,
// the first dimension represents the average loudness of the segment; second emphasizes brightness; third is more closely correlated to the flatness of a sound;
// fourth to sounds with a stronger attack; etc....   The actual timbre of the segment is best described as a linear combination of these 12 basis functions weighted by the coefficient values:
//  timbre = c1 x b1 + c2 x b2 + ... + c12 x b12, where c1 to c12 represent the 12 coefficients and b1 to b12 the 12 basis functions as displayed below.
// Timbre vectors are best used in comparison with each other."
// http://developer.echonest.com/docs/v4/_static/AnalyzeDocumentation.pdf

static const int kTimbreVectorSize  = 12;

class ENAnalysisResult{
    
public:
    ENAnalysisResult(ofxJSONElement json);
    ~ENAnalysisResult();

    ofxJSONElement *getSegments();

private:
    ofxJSONElement *segments;
};

#endif /* defined(__echonest__ENAnalysisResult__) */
