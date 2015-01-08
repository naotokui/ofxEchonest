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

class ENAnalysisResult{
    
public:
    ENAnalysisResult(ofxJSONElement json);
    ~ENAnalysisResult();

    ofxJSONElement *getSegments();

private:
    ofxJSONElement *segments;
};

#endif /* defined(__echonest__ENAnalysisResult__) */
