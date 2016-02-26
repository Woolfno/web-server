#ifndef _HTTP_H
#define _HTTP_H

#include <iostream>
#include <string>
#include <vector>

const std::vector<std::string> explode(const std::string & s, const char& c);
std::string get(const std::vector<std::string>& request);
std::string http_request(const char * request,const char * directory);

#endif
