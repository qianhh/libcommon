#include "StdAfx.h"
#include "RegexHelper.h"
#include "pcre/pcrs.h"
#include "log.h"

namespace eric { namespace common {
//  PCRE ģʽ���η�
// 
// 	ģʽ���η�
// 	i PCRE_CLASSES
// 	��Сд�����е����η�
// 	m PCRE_MULTILINE
// 	����ƥ��, Ĭ�������, PCRE��ΪĿ���ַ�����һ�鵥���ַ���ɵ�(Ȼ��ʵ���������ܻ��������). Ԫ�ַ�^��ƥ���ַ����Ŀ�ʼλ��, ��Ԫ�ַ�$���ƥ���ַ�����ĩβ, ��������֮ǰ(����������D���η�). ������η���perl�й���һֱ, ʹ�ú�, ^��$��ƥ��ÿһ�еĿ�ʼ�ͽ�β
// 	���Ŀ���ַ�����û��/n�ַ�����ģʽ��û�г���^��$, �����ô����η���û���κ�Ӱ���.
// 	s PCRE_DOTALL
// 	������ǿ�ַ���.(���)�Ĺ���, Ĭ�������, ���ƥ�������ַ�, �����������з����Ժ�����. ʹ�ô����η���, ����ƥ�������ַ��������з�
// 	x PCRE_EXTENDED
// 	���������������η�, ģʽ�еĿհ��ַ�, ���Ǳ�ת�������һ���ַ�����, ���򶼱�����. ��һ��δת���#֮��ֱ����һ�����з�֮������ݶ��ᱻ������ʽ��������Է������ע��.
// 	e PREG_REPLACE_EVAL
// 	ʹ�ô����η���, preg_replace��$replacement�������˿���ʹ�������ķ���������ȡ��ƥ��ֵ, ��������������дeval�﷨������ַ���������ֵ, �������ؽ���������յ��滻.
// 	A PCRE_ANCHORED
// 	��������˴����η�, ģʽ��ǿ�Ƴ�Ϊ��anchored��(ê��), Ҳ����˵Լ��ʹ��ʼ�մ��ַ�������ǰ�濪ʼ����. ���Ӱ��Ҳ����ͨ�������ʵ��Ĺ�����ʵ��(perl��ֻ������ʵ��)
// 	D PCRE_DOLLAR_ENDONLY
// 	���������������η�, $��ƥ��Ŀ���ַ����Ľ�β֮ǰ, �����û������������η�, ���Ŀ���ַ������һ���ַ�ʱ���з�, $�ͻ�ƥ��û��з�֮ǰ�������ַ�����β֮ǰ.
// 	���������m���η�, ��������η��ᱻ����. ��һ���perl��һ��
// 	S
// 	��һ��ģʽ��Ҫ���ʹ�õ�ʱ��, ����Ϊ�˻�ø��ߵ�ƥ��Ч��, ֵ�û���һЩʱ�������з���. ���������������η�, ��������������ķ���. ��ǰ, ���ֶ�һ��ģʽ�ķ������������ڷ�ê���ƥ��(��û��һ����һ�̶��Ŀ�ʼ�ַ�)
// 	U PCRE_UNGREEDY
// 	������η���ת��̰����ģʽ, ���û���������, Ĭ������µ�����ƥ��ʱ̰����, ���ʺ�������?��ģʽ�Ƿ�̰����, �������������������η�, ��պ��෴.
// 	���������perl�ǲ����ݵ�.
// 	Ҳ������ģʽ��ʹ��?U���ﵽͬ����Ч��
// 	X PCRE_EXTRA
// 	������η�������һ��PCRE����perl�����ݵĶ��⹦��. ���ⷴб�ߺ����һ��û�����⺬����ַ��ᵼ��һ������, �Դ���������Щ����Ա�������չ.
// 	Ĭ�������, ��perlһ��, ��б�ߺ��һ��û�����⺬����ַ����Ը��ַ�ԭ�����.
// 	��ǰû���κ����������ܴ����η�����
// 	J PCRE_INFO_JCHANGED
// 	���ڲ�ѡ��?J��ͬ, �����ı䱾��PCRE_DUPNAMESѡ��. �����������ظ�������
// 	u PCRE8
// 	������η���һ��PCRE����perl�����ݵĶ��⹦��. ģʽ�ַ�������Ϊ��UTF-8��.



//regex: Input Param ������"/"��ͷ�ͽ���,Ҫƥ�䣬����/img*/
//input:   Input Param Դ�ַ���
//replaced:Input Param �滻�Ӵ�
//output:  Output Param Ŀ���ַ���
//incase:  Input Param true ��Сд������ 
//global:  Input Param true ȫ���滻
//greedy:  Input Param true ̰��ģʽ
//hits:	   Out Param �ɹ��滻���� �����������
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

//����������ʽƥ�䲢��ȡƥ�䵽���Ӵ�
//regex:   Input Param ������ʽ
//input:   Input Param Դ�ַ���
//output:  Output Param ƥ�䵽���Ӵ�����
//incase:  Input Param true ��Сд������ 
//global:  Input Param true ȫ���滻
//greedy:  Input Param true ̰��ģʽ
//hits:	   Out Param �ɹ�ƥ�䵽�ĸ��� �����������
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
