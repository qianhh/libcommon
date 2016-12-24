#pragma once
#include <string>
#include <vector>

/**
 ** pcre正则表达式操作
 ** designed by eric,2016/11/02
 **/
namespace eric { namespace common {
class RegexHelper
{
public:
	//根据正则表达式匹配并替换成目标子串
	//regex:   Input Param 必须以"/"开头和结束,要匹配，例如/img*/
	//input:   Input Param 源字符串
	//replaced:Input Param 替换子串
	//output:  Output Param 目标字符串
	//incase:  Input Param true 大小写不敏感 
	//global:  Input Param true 全局替换
	//greedy:  Input Param true 贪婪模式
	//return： 成功替换个数 负数代表错误
	//example: PcreRegexHelper::Replace("/script/","i am a script script script file","r",example);
	static int Replace(std::string regex,std::string input,std::string replaced,std::string& output,bool incase = true,bool global = true,bool greedy =true);

	//根据正则表达式匹配并获取匹配到的子串
	//regex:   Input Param 正则表达式
	//input:   Input Param 源字符串
	//output:  Output Param 匹配到的子串数组
	//incase:  Input Param true 大小写不敏感 
	//global:  Input Param true 全局替换
	//greedy:  Input Param true 贪婪模式
	//return： 成功匹配到的子串数 负数代表错误
	//example: PcreRegexHelper::Match("script","i am a script Script script file",vecResults);
	static int Match(std::string regex,std::string input,std::vector<std::string>& output,bool incase = true,bool global = true,bool greedy =true);
};

}}
