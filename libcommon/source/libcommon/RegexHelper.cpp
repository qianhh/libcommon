#include "StdAfx.h"
#include "RegexHelper.h"
#include "pcre/pcrs.h"
#include "log.h"

namespace eric { namespace common {
//  PCRE 模式修饰符
// 
// 	模式修饰符
// 	i PCRE_CLASSES
// 	大小写不敏感的修饰符
// 	m PCRE_MULTILINE
// 	多行匹配, 默认情况下, PCRE认为目标字符串是一组单行字符组成的(然而实际上它可能会包含多行). 元字符^仅匹配字符串的开始位置, 而元字符$则仅匹配字符串的末尾, 或者新行之前(除非设置了D修饰符). 这个修饰符和perl中工作一直, 使用后, ^和$会匹配每一行的开始和结尾
// 	如果目标字符串中没有/n字符或者模式中没有出现^和$, 则设置此修饰符是没有任何影响的.
// 	s PCRE_DOTALL
// 	用来增强字符类.(点号)的功能, 默认情况下, 点号匹配任意字符, 但不包括换行符及以后内容. 使用此修饰符后, 可以匹配任意字符包括换行符
// 	x PCRE_EXTENDED
// 	如果设置了这个修饰符, 模式中的空白字符, 除非被转义或者在一个字符类中, 否则都被忽略. 在一个未转义的#之后直到下一个换行符之间的内容都会被正则表达式引擎忽略以方便添加注释.
// 	e PREG_REPLACE_EVAL
// 	使用此修饰符后, preg_replace的$replacement参数除了可以使用正常的反向引用来取得匹配值, 还可以在其中书写eval语法允许的字符串进行求值, 并将返回结果用于最终的替换.
// 	A PCRE_ANCHORED
// 	如果设置了此修饰符, 模式被强制成为”anchored”(锚点), 也就是说约束使其始终从字符串的嘴前面开始搜索. 这个影响也可以通过构造适当的规则来实现(perl中只能这样实现)
// 	D PCRE_DOLLAR_ENDONLY
// 	如果设置了这个修饰符, $会匹配目标字符串的结尾之前, 而如果没有设置这个修饰符, 如果目标字符串最后一个字符时换行符, $就会匹配该换行符之前而不是字符串结尾之前.
// 	如果设置了m修饰符, 则这个修饰符会被忽略. 这一点和perl不一致
// 	S
// 	当一个模式需要多次使用的时候, 我们为了获得更高的匹配效率, 值得花费一些时间对其进行分析. 如果设置了这个修饰符, 将会进行这个额外的分析. 当前, 这种对一个模式的分析仅仅适用于非锚点的匹配(即没有一个单一固定的开始字符)
// 	U PCRE_UNGREEDY
// 	这个修饰符逆转了贪婪的模式, 如果没有这个设置, 默认情况下的正则匹配时贪婪的, 量词后增加了?的模式是非贪婪的, 但是如果设置了这个修饰符, 则刚好相反.
// 	这个规则与perl是不兼容的.
// 	也可以在模式中使用?U来达到同样的效果
// 	X PCRE_EXTRA
// 	这个修饰符启用了一个PCRE中与perl不兼容的额外功能. 任意反斜线后面跟一个没有特殊含义的字符会导致一个错误, 以此来保留这些组合以备后期扩展.
// 	默认情况下, 和perl一样, 反斜线后跟一个没有特殊含义的字符会以该字符原意解释.
// 	当前没有任何其他特性受此修饰符控制
// 	J PCRE_INFO_JCHANGED
// 	与内部选项?J相同, 用来改变本地PCRE_DUPNAMES选项. 允许子组有重复的名字
// 	u PCRE8
// 	这个修饰符打开一个PCRE中与perl不兼容的额外功能. 模式字符串被认为是UTF-8的.



//regex: Input Param 必须以"/"开头和结束,要匹配，例如/img*/
//input:   Input Param 源字符串
//replaced:Input Param 替换子串
//output:  Output Param 目标字符串
//incase:  Input Param true 大小写不敏感 
//global:  Input Param true 全局替换
//greedy:  Input Param true 贪婪模式
//hits:	   Out Param 成功替换个数 负数代表错误
//example: s/(<script.*)nasty-item(?=.*<\/script>)/$1replacement/sigU
int RegexHelper::Replace(std::string regex,std::string input,std::string replaced,std::string& output,bool incase,bool global,bool greedy)
{
	int hits = 0;
	if(input.empty() || regex.empty()){
		return hits;
	}
	regex = "s" + regex + replaced + "/";
	if(incase){
		regex +="i";
	}
	if(global){
		regex +="g";
	}
	if(greedy){
		regex +="U";
	}
	char* pcrs_command_tmp = pcrs_subsitute_command(input.c_str(),regex.c_str(),&hits);
	if (NULL == pcrs_command_tmp)
	{
		LOG_TRACE("nothing replaced,input:%s regex:%s", input.c_str(), regex.c_str());
		return hits;
	}
	output = std::string(pcrs_command_tmp);
	free(pcrs_command_tmp);
	return hits;
}

//根据正则表达式匹配并获取匹配到的子串
//regex:   Input Param 正则表达式
//input:   Input Param 源字符串
//output:  Output Param 匹配到的子串数组
//incase:  Input Param true 大小写不敏感 
//global:  Input Param true 全局替换
//greedy:  Input Param true 贪婪模式
//hits:	   Out Param 成功匹配到的个数 负数代表错误
//example: CRegexHelper::Match("script","i am a script Script script file",vecResults);
int RegexHelper::Match(std::string regex,std::string input,std::vector<std::string>& output,bool incase,bool global,bool greedy)
{
	output.clear();
	int hits = 0;
	if(input.empty() || regex.empty()){
		return hits;
	}
	int options = 0;
	if(incase){
		options |= PCRE_CASELESS;
	}
	if(greedy){
		options |= PCRE_UNGREEDY;
	}

	const char *error = NULL;
	int errptr = 0;
	const int OVECCOUNT = 99;
	int ovector[OVECCOUNT] = {0};
	const pcre* pattern = NULL;

	pattern = pcre_compile(regex.c_str(), options, &error, &errptr, NULL);
	if (pattern == NULL)
	{
		LOG_TRACE("pcre_compile is error[%s], pattern is null.", (error?error:"null"));
		return hits;
	}

	const char *subject = input.c_str();
	int length = (int)input.length();
	int i = 0;
	int submatches = 0;
	int offset = 0;
	std::string strTemp;
	while ((submatches = pcre_exec(pattern, NULL, subject, length, offset, 0, ovector, OVECCOUNT)) > 0)
	{
		hits += submatches;
		for (i = 0; i < submatches; i++)
		{
			char *substring_start = (char *)subject + ovector[2*i];
			offset = ovector[2*i+1];
			int substring_length = offset - ovector[2*i];
			strTemp.clear();
			strTemp.append(substring_start, substring_length);
			output.push_back(strTemp);
		}

		if (!global) break;
	}
	pcre_free((void *)pattern);
	/* Pass pcre error through if (bad) failiure */
	if (submatches < PCRE_ERROR_NOMATCH)
	{
		return submatches;
	}
	return hits;
}

}}
