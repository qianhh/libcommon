#pragma once
#include <string>

//base64����
std::string Encode64(const unsigned char* Data, int DataByte);

//base64����
std::string Decode64(const char* Data, int DataByte, int& OutByte);

