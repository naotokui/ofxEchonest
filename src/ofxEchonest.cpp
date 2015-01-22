//
//  ofxEchonest.cpp
//  echonest
//
//  Created by Nao Tokui on 11/12/14.
//  Copyright (c) 2014 Nao Tokui <www.naotokui.net>. All rights reserved.
//

#include "ofxEchonest.h"
#include "ofxCurl.h"
#include "ofUtils.h"
#include "ofMain.h"
#include <map>

static const std::string kEchonestAPIBaseURL = "http://developer.echonest.com/api/v4";

#pragma mark
#pragma mark CONSTRUCTOR

ofxEchonest::ofxEchonest(const std::string apiKey, const std::string consumerKey)
{
    // Store API Keys
    _echonestApiKey = apiKey;
    _echonestConsumerKey = consumerKey;
    
    analysis = NULL;
    
    ofAddListener(httpUtils.newResponseEvent,this,&ofxEchonest::newResponse);
}

ofxEchonest::~ofxEchonest()
{
    if (analysis) delete analysis;
}

#pragma mark
#pragma mark API CALL

// spotifyId should be something like spotify:track:50IhWrjEoHJFFDrVpAcUus
bool ofxEchonest::getSongProfile(string spotifyId){
    string apiUrl = kEchonestAPIBaseURL + "/song/profile?api_key=" + _echonestApiKey + "&track_id="+ spotifyId + "&bucket=audio_summary";
    httpUtils.getUrl(apiUrl);
}

void ofxEchonest::newResponse(ofxHttpResponse & response){
    string responseStr = ofToString(response.status) + ": " + (string)response.responseBody;
    ofLog(OF_LOG_NOTICE) << responseStr;
    ofxJSONElement json;
    json.parse(response.responseBody);
    ofLog(OF_LOG_NOTICE) << json;
}


#pragma mark

/* Caution: this function does not return immediately */
bool ofxEchonest::uploadAndAnalyze(string filepath){
    string apiUrl = kEchonestAPIBaseURL + "/track/upload";
    
    // Checking audio file type based on the file extention
    string extention = filepath.substr(filepath.find_last_of(".") + 1);
    string filetype = "wav";
    if (extention == "aif" || extention == "aiff") filetype = "wav"; /// TODO
    else if (extention == "wav") filetype = "wav";
    else if (extention == "mp3") filetype = "mp3";
    else {
        ofLogNotice("ofxEchonest", "Invalid file?");
        return;
    }
    
    // Create a form.
    ofxCurl curl;
    ofxCurlForm* form = curl.createForm(apiUrl);
    form->setTimeout(1000 * 1800.0); // timeout = 1min
    form->addInput("filetype", filetype);
    form->addInput("api_key", _echonestApiKey);
    
    // Add some form entries.
    ofLog(OF_LOG_VERBOSE) << "Start uploading...  " <<  filepath;
    form->addFile("track", filepath);  // curl
    
    // Perform the post.
    try {
        form->post();
    }
    catch(ofxCurlException exception) {
        ofLog(OF_LOG_ERROR) << "OOPS.. something went wrong while posting " + exception.getMessage();
    }
    delete form;
    
    // Do something with the response from the post.
    vector<char> response_buf = form->getPostResponseAsBuffer();
    string response_str = form->getPostResponseAsString();
    ofLog(OF_LOG_VERBOSE) << "Size of response buffer: " << response_buf.size();
    ofLog(OF_LOG_VERBOSE) << "Response string:";
    ofLog(OF_LOG_VERBOSE) << "-----------------";
    ofLog(OF_LOG_VERBOSE) << response_str;
    
    ofxJSONElement json;
    if (response_str.length() > 0){
        if (json.parse(response_str)){
            if (json["response"]["status"]["code"]==0){
                string status = json["response"]["track"]["status"].asString();
                if (status == "pending"){
                    string md5 = json["response"]["track"]["md5"].asString();
                    return waitUntilAnalysisFinishes(md5);
                } else if (status == "complete"){
                    string analysis_url = json["response"]["track"]["audio_summary"]["analysis_url"].asString();
                    if (analysis_url.length() > 0){
                        return getAudioAnalysis(analysis_url);
                    }
                }
                
            }
        }
    }
    return false;
}

bool ofxEchonest::waitUntilAnalysisFinishes(string md5){
    int count = 0;
    while(1){
        string url = kEchonestAPIBaseURL + "/track/profile?format=json&bucket=audio_summary&api_key=" + _echonestApiKey + "&md5=" + md5;
        
        // Now parse the JSON
        bool parsingSuccessful = json.open(url);
        if (parsingSuccessful)
        {
            ofLogVerbose("ofxEchonest::waitUntilAnalysisFinishes ") << json.getRawString(true);
            string analysis_url = json["response"]["track"]["audio_summary"]["analysis_url"].asString();
            if (analysis_url.length() > 0){
                
                sleep(1);
                bool success = getAudioAnalysis(analysis_url);
                if (success) {
                    return true;
                }
            }
        } else {
            ofLogNotice("waitUntilAnalysisFinishes") << "Failed to parse JSON.";
            break;
        }
        if (count++ > 2){
            ofLog(OF_LOG_ERROR) << "Analysis Timeout" << endl;
            break;
        }
        sleep(5);
    }
    return false;
}

bool ofxEchonest::getAudioAnalysis(string analysis_url){
    bool parsingSuccessful = json.open(analysis_url);
    if (parsingSuccessful)
    {
        ofLogVerbose("ofxEchonest::getAudioAnalysis ") << json.getRawString(true);
        
        //        if (analysis != NULL) delete analysis;  // TODO: remove analysis
        analysis = new ENAnalysisResult(json);
        return true;
    } else {
        ofLogError("ofApp::audioAnalysis") << "Failed to parse JSON.";
        return false;
    }
}

bool ofxEchonest::hasAnalysisResult(){
    return (analysis != NULL);
}

ENAnalysisResult* ofxEchonest::getAnalysisResult(){
    return analysis;
}

#pragma mark
#pragma mark UTILITY

bool ofxEchonest::isAPIKeySet(){
    if (_echonestApiKey.length() > 0 && _echonestConsumerKey.length() > 0) return true;
    return false;
}



