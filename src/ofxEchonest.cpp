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
}

ofxEchonest::~ofxEchonest()
{
    cout << "~ofxEchonest() called" << endl;
    if (analysis) delete analysis;
}

#pragma mark
#pragma mark API CALL


#pragma mark
#pragma mark UTILITY

bool ofxEchonest::isAPIKeySet(){
    if (_echonestApiKey.length() > 0 && _echonestConsumerKey.length() > 0) return true;
    return false;
}


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
//    string filepath = ofToDataPath(filename, true);
    cout << "Start uploading..." << endl;
    cout << filepath << endl;
    form->addFile("track", filepath);  // curl
    
    // Perform the post.
    try {
        form->post();
    }
    catch(ofxCurlException exception) {
        cout << "OOPS.. something went wrong while posting " + exception.getMessage() << endl;
    }
    delete form;
    
    // Do something with the response from the post.
    vector<char> response_buf = form->getPostResponseAsBuffer();
    string response_str = form->getPostResponseAsString();
    cout << "Size of response buffer: " << response_buf.size() << endl << endl;
    cout << "Response string:" << endl;
    cout << "-----------------" << endl;
    cout << response_str <<endl;
    
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
            ofLogNotice("waitUntilAnalysisFinishes") << json.getRawString(true);
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
            cout << "Analysis Timeout" << endl;
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
        ofLogNotice("ofApp::audioAnalysis") << json.getRawString(true);

        if (analysis != NULL) delete analysis;
        analysis = new ENAnalysisResult(json);
        return true;
    } else {
        ofLogNotice("ofApp::audioAnalysis") << "Failed to parse JSON.";
        return false;
    }
}

bool ofxEchonest::hasAnalysisResult(){
    return (analysis != NULL);
}

ENAnalysisResult* ofxEchonest::getAnalysisResult(){
    return analysis;
}
