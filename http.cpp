#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

static const char * home_directory;

const std::vector<std::string> explode(const std::string& s, const char& c){
	std::string buff{""};
	std::vector<std::string> v;
	
	for(auto n:s){
		if(n != c) buff+=n; 
		else if(n == c && buff != ""){
			v.push_back(buff); buff = ""; 
		}
	}
	
	if(buff != "") v.push_back(buff);
	
	return v;
}

std::string get(const std::vector<std::string>& request){
	std::stringstream response;
	std::stringstream response_body;

	std::string path=std::string(home_directory)+request[1];
	if(request[1]=="/")	
		path+="index.html";

	std::ifstream fin(path.c_str());
	if(!fin.is_open()){
		response<<"HTTP/1.0 404 Not Found\r\n"
				<<"Content-Type: text/html; charset=utf-8\r\n"
				<<"Content-Length: "<< response_body.str().length()
				<<"\r\n\r\n"
				<<response_body.str();
		return response.str();
	}

	response_body<<fin.rdbuf();
	fin.close();
	response<<"HTTP/1.0 OK\r\n"
			<<"Version: HTTP/1.1\r\n"
			<<"Content-Type: text/html; charset=utf-8\r\n"
			<<"Content-Length: "<< response_body.str().length()
			<<"\r\n\r\n"
			<<response_body.str();

	return response.str();
}

std::string http_request(const char * request,const char * directory){
	home_directory=directory;
	std::vector<std::string> request_v=explode(std::string(request),' ');
	if(request_v[0]=="GET")
		return get(request_v);

	return "";
}
