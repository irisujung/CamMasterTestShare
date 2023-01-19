#pragma once

#include <vector>
using namespace std;

namespace StringUtil
{
	static void Split(const CString& strString, const CString& strSeparation, vector<CString>& vecWord )
	{
		vecWord.clear();

		CString strText = strString;

		strText.Replace( _T(" "), _T("") );

		long nStart = 0, nPos = 0;

		while (TRUE)
		{
			nPos = strText.Find( strSeparation, nStart );

			if(nPos == -1)
			{
				CString strWord = strText.Mid(nStart, strText.GetLength() - nStart);
				if(strWord != _T("") )
				{
					vecWord.push_back(strWord);
				}			
				break;
			}

			vecWord.push_back(strText.Mid(nStart, nPos - nStart));

			nStart = nPos + 1;
		}	
	};


	static CString GetStringToFindChar(const CString& strString, const CString& strSeparation)
	{
		long nPos = strString.Find( strSeparation, 0) + 1;

		return strString.Mid(nPos, strString.GetLength() - nPos);
	}

};
