#pragma once
#include <string>
#include <vector>

/**
 ** pcre������ʽ����
 ** designed by eric,2016/11/02
 **/
namespace eric { namespace common {
class RegexHelper
{
public:
	//����������ʽƥ�䲢�滻��Ŀ���Ӵ�
	//regex:   Input Param ������"/"��ͷ�ͽ���,Ҫƥ�䣬����/img*/
	//input:   Input Param Դ�ַ���
	//replaced:Input Param �滻�Ӵ�
	//output:  Output Param Ŀ���ַ���
	//incase:  Input Param true ��Сд������ 
	//global:  Input Param true ȫ���滻
	//greedy:  Input Param true ̰��ģʽ
	//return�� �ɹ��滻���� �����������
	//example: PcreRegexHelper::Replace("/script/","i am a script script script file","r",example);
	static int Replace(std::string regex,std::string input,std::string replaced,std::string& output,bool incase = true,bool global = true,bool greedy =true);

	//����������ʽƥ�䲢��ȡƥ�䵽���Ӵ�
	//regex:   Input Param ������ʽ
	//input:   Input Param Դ�ַ���
	//output:  Output Param ƥ�䵽���Ӵ�����
	//incase:  Input Param true ��Сд������ 
	//global:  Input Param true ȫ���滻
	//greedy:  Input Param true ̰��ģʽ
	//return�� �ɹ�ƥ�䵽���Ӵ��� �����������
	//example: PcreRegexHelper::Match("script","i am a script Script script file",vecResults);
	static int Match(std::string regex,std::string input,std::vector<std::string>& output,bool incase = true,bool global = true,bool greedy =true);
};

}}
