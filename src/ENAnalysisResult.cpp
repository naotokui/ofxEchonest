//
//  ENAnalysisResult.cpp
//  echonest
//
//  Created by Nao Tokui on 11/13/14.
//  Copyright (c) 2014 Nao Tokui <www.naotokui.net>. All rights reserved.
//

#include "ENAnalysisResult.h"

static float kConfidenceThresh = 0.50;

ENAnalysisResult::ENAnalysisResult(ofxJSONElement json){
    
    ofxJSONElement _segments = json["segments"];
    
    cout << "# of segments "  << _segments.size() << endl;
    float totalTime = 0.0;
    for(unsigned int i = 0; i < _segments.size(); ++i)
    {
        float start  = _segments[i]["start"].asFloat();
        float duration  = _segments[i]["duration"].asFloat();
        cout << "segment #" << i << ": " << start << " - " << duration << " - " << _segments[i]["confidence"] << endl;
        totalTime += duration;
    }
    cout << "total time  " << totalTime << endl;
    
    // Filter segments with low confidence
    ofxJSONElement *segs = new ofxJSONElement();
    int nextIndex= -1;
    for(unsigned int i = 0; i < _segments.size(); i++)
    {
        ofxJSONElement segment = _segments[i];
        float duration = segment["duration"].asFloat();
        
        for(unsigned int j = i + 1; j < _segments.size(); j++){
            ofxJSONElement next = _segments[j];
            if (next["confidence"].asFloat() < kConfidenceThresh && j != _segments.size() - 1){
                duration += next["duration"].asFloat();
            } else {
                i = j - 1;
                break;
            }
        }
        segment["duration"] = duration;
        segs->append(segment);
    }
    segments = segs;
    
    cout << "<<<<<<<<<<<<<<<<<<<<" << endl;
    cout << "# of segments " << segments->size() << endl;
    totalTime = 0.0;
    for(unsigned int i = 0; i < segments->size(); ++i)
    {        
        ofxJSONElement element = segments->get(i, NULL);
        float start  = element["start"].asFloat();
        float duration  = element["duration"].asFloat();
        cout << "segment #" << i << ": " << start << " - " << duration << " - " << element["confidence"] << endl;
        totalTime += duration;
    }
    cout << "total time  " << totalTime << endl;
}

ENAnalysisResult::~ENAnalysisResult(){
    delete segments;
}

ofxJSONElement* ENAnalysisResult::getSegments(){
    return segments;
}