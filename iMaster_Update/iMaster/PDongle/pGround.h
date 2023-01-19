#pragma once


#ifdef CREATEDLL_EXPORTS
#define PGROUND_DECLSPEC __declspec(dllexport)
#else
#define PGROUND_DECLSPEC __declspec(dllimport)
#endif


extern "C" PGROUND_DECLSPEC bool CheckDongle(void);


class pGround
{
	pGround();
	~pGround();
};