
#ifndef _Util_H_
#define _Util_H_

// includes, system

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>	//rand
#include <assert.h>
#include <math.h>	//ceil
#include <string.h>
#include <fstream>
#include <iostream>
#include <fcntl.h> 
#include <conio.h>
#include <io.h>

using namespace std;

#define _bDebug 0

//////////////////////////////////////////////////////////////////////////types 
#define _nDim 4
#define _nMea 4 //num of measures
typedef int DimAttr[_nDim]; //stores leaf id (num in scales) along each dimension, range [0,nScale] for display
typedef int MeaAttr[_nMea]; //measures
#define _MaxMea0 12 //by Olap Council APB Data, max possible measure
#define _MaxMea1 700

enum AggrType
{
	AggrSum = 0,
	AggrCnt,
	AggrAvg,
	AggrMax,
	AggrMin,
};
static WCHAR* c_AggrNms[5] = {L"SUM", L"CNT", L"AVG", L"MAX", L"MIN"};

class OlapSchema
{
public:
	OlapSchema();
	~OlapSchema(){SAFE_DELETE_ARRAY(pDim); SAFE_DELETE_ARRAY(pMea);}
	void LoadData(char* fn, int nRec);
	WCHAR dimNames[_nDim][16];
	WCHAR meaNames[_nMea][16];
	int nLevels[_nDim];
	WCHAR levelNames[_nDim][10][16];
	int nScales[_nDim][10]; //num of categories from root to leaf
	int fanouts[_nDim][10]; //num of branches from root to leaf
	int nRec; //num of records
	DimAttr* pDim; //dim attribues
	MeaAttr* pMea; //measure
	int curLevels[_nDim]; //current displayed&manipulated level
	AggrType aggrType;
	
//display related
	int showAt[3]; //which dimension attribute to show at each axis
	int meaAt; //ball luminance as measure
	int nScaleCur[3]; //num of scales at current level in each dimension
	int scaleRatioCur[3]; //since the dim value is in [0, nScales[rootLevel]], we need rescale the value s.t. it fits [0, nScales[curLevel]]
	void OnShowAtUpdated(int axisId);
	//dice
	bool bDice[3]; //whether each shown dimension is checked for dicing
	bool bDiceSel; //whether the dice is selected out of original data
	int diceS[3]; //starting scale of the dice in each dimension
	int diceE[3]; //ending scale of the dice in each dimension
	void ResetDiceSE(int axId){diceS[axId] = 0; diceE[axId] = nScaleCur[axId];}
	void MoveDicePlane(int planeId, int delta);
};

//////////////////////////////////////////////////////////////////////////defines 

#ifndef mymax
#define mymax(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef mymin
#define mymin(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define myswap(a, b) {int tmp = a; a = b; b = tmp;}
#define _K(x) (x * 1024)
#define _M(x) (x * 1048576)
#define _Square(x) ((x) * (x))

static HRESULT hr = S_OK;

#define SAFE_FREE(p) {if(p) {free(p); (p)=NULL;} };

//#define Rand0_15bits(maxVal) (rand() % (maxVal))	//0-15bits
//#define Rand15_30bits(maxVal) (((rand()<<15) + rand()) % (maxVal))	//15 + 15 = 30bits
//#define Rand30_32bits(maxVal) (((rand()<<30) + (rand()<<15) + rand()) % (maxVal)) //2+15+15=32bits
#define Rand32(maxVal) (((rand()<<30) + (rand()<<15) + rand()) % (maxVal)) //can generate 0~32bits
#define Rand31(maxVal) ((((rand() & 1)<<30) + (rand()<<15) + rand()) % (maxVal)) //can generate 0~31bits

////////////////////////////////////////////////////////////////////////// funcs

//randomize 2 times on average
template<class _T>
void	Randomize(_T* R, const int nR)
{
	for(int i = 0; i < nR * 2; ++i)
		swap(R[rand() % nR], R[rand() % nR]);
}

//////////////////////////////////////////////////////////////////////////for debug

//bool WritePFMRaw(const char *fn, D3DXVECTOR3* pRawData, int pfmW, int pfmH);
//for test use. rawFpp: floats per pixel in pRawData. if 1, then write grayscale; 3/4, color; 4,skip alpha.
bool WritePFM_Floats(const char *fn, float* pRawData, int pfmW, int pfmH, int rawFpp);

//for test use. test byte, e.g. visibs. only write single channel
bool WritePFM_Bytes(const char *fn, byte* pRawData, int pfmW, int pfmH);

//write int3/int4 data into bmp. skip alpha for int4.
bool WriteBmp_Ints(const char *fn, int* pRawData, int W, int H, int nChannel);

//write int3/int4 data into bmp. skip alpha for int4.
bool WriteBmp_Bytes(const char *fn, byte* pRawData, int W, int H, int nChannel);

unsigned int uintFloorLog(unsigned int base, unsigned int num);

unsigned int uintCeilingLog(unsigned int base, unsigned int num);

unsigned int uintPower(unsigned int base, unsigned int pow);

unsigned int uintCeilingDiv(unsigned int dividend, unsigned int divisor);

unsigned int tmpCeilingLog(unsigned int base, unsigned int num);

unsigned int tmpPow(unsigned int base, unsigned int p);

int tmpTranslate(unsigned int treeIdx, unsigned int TS, unsigned int BS, unsigned int AS);

#endif
