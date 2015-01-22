//
//  ofxEchonest.h
//  echonest
//
//  Created by Nao Tokui on 11/12/14.
//  Copyright (c) 2014 Nao Tokui <www.naotokui.net>. All rights reserved.
//

#ifndef __echonest__ofxEchonest__
#define __echonest__ofxEchonest__

#include <stdio.h>
#include <iostream>
#include <string>
#include "ofxJSON.h"
#include "ENAnalysisResult.h"
#include "ofxHttpUtils.h"

class ofxEchonest
{
public:
    ofxEchonest(const std::string apiKey, const std::string consumerKey);
    ~ofxEchonest();
    
    bool uploadAndAnalyze(string filename);
    bool waitUntilAnalysisFinishes(string md5);
    bool getAudioAnalysis(string analysis_url);
    
    bool hasAnalysisResult();
    ENAnalysisResult* getAnalysisResult();
    
    bool getSongProfile(string spotifyId);
    
private:
    std::string _echonestApiKey, _echonestConsumerKey;
    
    ofxHttpUtils httpUtils;
    void newResponse(ofxHttpResponse & response);
    
    ofxJSONElement json;
    ENAnalysisResult *analysis;
    
    bool isAPIKeySet(); // check if API Keys are already set and ready to call API
};

#endif /* defined(__echonest__ofxEchonest__) */
