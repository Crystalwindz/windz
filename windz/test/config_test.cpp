//
// Created by crystalwind on 19-1-18.
//

#include "windz/Config.h"
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <list>

#define PRINTLN(s) cout << #s" = " << (s) << endl

using namespace windz;
using namespace std;

void NormalTest() {
    Config config;
    int ret = config.Parse("./conf/normal.conf");
    assert(ret == 0);
    auto conf = config.config();
    for (auto &i: conf) {
        cout << i.first << endl;
        for (auto &j: i.second) {
            cout << j << endl;
        }
    }
    PRINTLN(config.GetString("section1", "a", "none"));
    PRINTLN(config.GetString("none", "none", "none"));

    PRINTLN(config.GetInt("int","d1",0));
    PRINTLN(config.GetInt("int","d2",0));
    PRINTLN(config.GetInt("int","d3",1));
    PRINTLN(config.GetInt("int","h1",0));
    PRINTLN(config.GetInt("int","h2",0));
    PRINTLN(config.GetInt("int","o1",0));
    PRINTLN(config.GetInt("int","o2",0));
    PRINTLN(config.GetInt("int","b1",0));
    PRINTLN(config.GetInt("int","none",0));

    PRINTLN(config.GetDouble("double","a",0));
    PRINTLN(config.GetDouble("double","b",0));
    PRINTLN(config.GetDouble("double","c",0));
    PRINTLN(config.GetDouble("double","d",0));
    PRINTLN(config.GetDouble("double","i",0));

    PRINTLN(config.GetBool("bool","true", false));
    PRINTLN(config.GetBool("bool","yes", false));
    PRINTLN(config.GetBool("bool","on", false));
    PRINTLN(config.GetBool("bool","1", false));
    PRINTLN(config.GetBool("bool","false",true));
    PRINTLN(config.GetBool("bool","no",true));
    PRINTLN(config.GetBool("bool","off",true));
    PRINTLN(config.GetBool("bool","0",true));
    PRINTLN(config.GetBool("bool","nfo",false));
}

void ErrorTest() {
    Config config;
    int ret = config.Parse("./conf/section_error.conf");
    assert(ret == 4);
    auto conf = config.config();
    for (auto &i: conf) {
        cout << i.first << endl;
        for (auto &j: i.second) {
            cout<< j<< endl;
        }
    }

    ret = config.Parse("./conf/comment_error.conf");
    assert(ret == 1);

}

int main()
{
    cout << "NormalTest:\n";
    NormalTest();
    cout << "\nErrorTest:\n";
    ErrorTest();
    return 0;
}