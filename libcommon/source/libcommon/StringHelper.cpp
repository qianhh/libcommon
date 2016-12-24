#include "stdafx.h"
#include "StringHelper.h"
#include <string>
#include <vector>
#include "RegexHelper.h"
using namespace std;

namespace eric { namespace common {
std::string StringHelper::ExtractInParenthesis(const std::string &input)
{
	std::vector<std::string> vecResults;
	RegexHelper::Match("(?<=\\().*(?=\\))", input, vecResults, false);
	return vecResults.size() > 0 ? vecResults[0] : "";
}

void StringHelper::ExtractInParenthesis(const std::string &input, std::vector<std::string> &output)
{
	RegexHelper::Match("(?<=\\().*(?=\\))", input, output, false);
}

template<class T, typename Type>
static void SplitStringT(const T &input, Type cMask, std::vector<T> &output)
{
	output.clear();
	long lOffset = 0;
	long lFindIndex = 0;
	const Type *pszInput = input.c_str();
	T temp;
	while ((lFindIndex = (long)input.find(cMask, lOffset)) >= 0)
	{
		temp.clear();
		temp.append(pszInput + lOffset, lFindIndex - lOffset);
		output.push_back(temp);
		lOffset = lFindIndex + 1;
	}
	if (lOffset < (long)input.size())
	{
		temp.clear();
		temp.append(pszInput + lOffset, input.size() - lOffset);
		output.push_back(temp);
	}
}

void StringHelper::SplitString(const std::string &input, char cMask, std::vector<std::string> &output)
{
	SplitStringT(input, cMask, output);
}

void StringHelper::SplitString(const std::wstring &input, wchar_t cMask, std::vector<std::wstring> &output)
{
	SplitStringT(input, cMask, output);
}

bool StringHelper::LTrim(wstring& wstr, wchar_t sp)
{
	if(!wstr.empty())
	{
		wchar_t* tmp_head = new wchar_t[wstr.length() + 1];
		memcpy(tmp_head,wstr.c_str(),wstr.length() * sizeof(wchar_t));
		tmp_head[wstr.length()] = '\0';


		wchar_t *p, *q, *r;

		p = tmp_head + wstr.length();

	   /*
		* find end of leading whitespace
		*/
	   q = r = tmp_head;
	   while (*q && (*q == sp))
	   {
		  q++;
	   }

	   /*
		* if there was any, move the rest forwards
		*/
	   if (q != tmp_head)
	   {
		  while (q <= p )
		  {
			 *r++ = *q++;
		  }
	   }
	   wstr.clear();
	   wstr = tmp_head;

	   delete tmp_head;
	}

   return true;
}

bool StringHelper::RTrim(wstring& wstr, wchar_t sp)
{
	if(!wstr.empty())
	{
		wchar_t* tmp_head = new wchar_t[wstr.length() + 1];
		memcpy(tmp_head,wstr.c_str(),wstr.length() * sizeof(wchar_t));
		tmp_head[wstr.length()] = '\0';
		/*
		* strip trailing whitespace
		*/
	   wchar_t* p = tmp_head + wstr.length();
	   while (p > tmp_head && *(p-1) == sp)
	   {
		  p--;
	   }
	   *p = '\0';

	   wstr.clear();
	   wstr = tmp_head;

	   delete tmp_head;
	}


	return true;
}

bool StringHelper::Trim(wstring& wstr, wchar_t sp)
{
	if(!wstr.empty())
	{
		wchar_t* tmp_head = new wchar_t[wstr.length() + 1];
		memcpy(tmp_head,wstr.c_str(),wstr.length() * sizeof(wchar_t));
		tmp_head[wstr.length()] = '\0';


		wchar_t *p, *q, *r;

	   /*
		* strip trailing whitespace
		*/
	   p = tmp_head + wstr.length();
	   while (p > tmp_head && *(p-1) == sp)
	   {
		  p--;
	   }
	   *p = '\0';

	   /*
		* find end of leading whitespace
		*/
	   q = r = tmp_head;
	   while (*q && (*q == sp))
	   {
		  q++;
	   }

	   /*
		* if there was any, move the rest forwards
		*/
	   if (q != tmp_head)
	   {
		  while (q <= p)
		  {
			 *r++ = *q++;
		  }
		}

	   wstr.clear();
	   wstr = tmp_head;

	   delete tmp_head;
	}

   return true;
}

void StringHelper::Replace(wstring &strInput, const wstring &strOld, const wstring &strNew)
{
    if (strInput.empty())
    {
        return;
    }

	wstring::size_type pos = 0;
	wstring::size_type srclen = strOld.size();         
	wstring::size_type dstlen = strNew.size();       
	while((pos = strInput.find(strOld, pos)) != wstring::npos)
	{ 
		strInput.replace(pos, srclen, strNew);                 
		pos += dstlen;         
	}
}

}}
