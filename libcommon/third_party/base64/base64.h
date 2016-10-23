#pragma once
#include <string>

//base64±àÂë
std::string Encode64(const unsigned char* Data, int DataByte);

//base64½âÂë
std::string Decode64(const char* Data, int DataByte, int& OutByte);

