//(c) Ke Yang, yk.cadcg@gmail.com


//includes, within project
#include "DXUT.h"
#include "Util.h"
#include "atlimage.h"	//writeBmp

/////////////////////////////////////////////////////////////////////Generate R, S

//bool WritePFMRaw(const char *fn, D3DXVECTOR3* pRawData, int pfmW, int pfmH)
//{
//	using namespace std;
//	ofstream output(fn, ios::binary);
//	if(!output.is_open())
//		return false;	
//
//	output << 'P'<<'F'<<endl;
//	output <<pfmW<<' '<<pfmH<<endl; 
//	output <<-1<<endl;
//
//	output.write((char*)pRawData, pfmW * pfmH * sizeof(D3DXVECTOR3));
//
//	return true;
//}

//for test use. rawFpp: floats per pixel in pRawData. if 1, then write grayscale; 3/4, color; 4,skip alpha.
bool WritePFM_Floats(const char *fn, float* pRawData, int pfmW, int pfmH, int rawFpp)
{
	using namespace std;
	ofstream output(fn, ios::binary);
	if(!output.is_open())
		return false;	
	if((rawFpp == 3)||(rawFpp == 4))	//color
		output << 'P'<<'F'<<endl;
	else	//gray
		output << 'P'<<'f'<<endl;
	output <<pfmW<<' '<<pfmH<<endl; 
	output <<-1<<endl;

	float* pCur = pRawData;
	float tmp;
	if(rawFpp == 1)
	{
		for(int i = 0; i < pfmW * pfmH; ++i)
		{
			tmp = (float)*pCur;
			output.write((char*)&tmp, 1 * sizeof(float));
			++pCur;
		}
	}
	else if(rawFpp == 3)
	{
		for(int i = 0; i < pfmW * pfmH * 3; ++i)	//color: 3 floats for each pixel
		{
			tmp = (float)*pCur;
			output.write((char*)&tmp, 1 * sizeof(float));
			++pCur;
		}
	}
	else if(rawFpp == 4)
	{
		for(int i = 0; i < pfmW * pfmH * 4; ++i)	//color: 3 floats for each pixel
		{
			if(i % 4 != 3)	//skip alpha
			{
				tmp = (float)*pCur;
				output.write((char*)&tmp, 1 * sizeof(float));
			}
			++pCur;
		}
	}
	return true;
}

//write int3/int4 data into bmp. skip alpha for int4.
bool WriteBmp_Bytes(const char *fn, byte* pRawData, int W, int H, int nChannel)

{
	if((!pRawData) || ((nChannel != 3) && (nChannel != 4))	)
	{
		assert(0);
		return false;
	}

	//for test load pfm
	CImage img;
	if(!img.Create(W, H, 24))
	{
		assert(0);
		return false;
	}
	BYTE* pBits = static_cast<BYTE*>(img.GetBits());
	int p = img.GetPitch(); //pitch sometimes is negative
	byte* pV = pRawData;
	for (int j = 0; j < H; ++j)
	{
		for (int i = 0; i < W; ++i)
		{
			BYTE* pixAddr = pBits + p * j + i * img.GetBPP() / 8;	//GetBPP: The number of bits per pixel.
			//access pix here. Byte sequence is BGR for 24BPP image.
			*(pixAddr + 2) = (byte)*(pV	   );	//r
			*(pixAddr + 1) = (byte)*(pV + 1);	//g
			*(pixAddr    ) = (byte)*(pV + 2);	//b
			pV += nChannel;
		}
	}

	CString fn1 = (CString)(LPSTR)fn;
	img.Save(fn1 + ".bmp");
	img.Destroy();
	return true;
}

//write int3/int4 data into bmp. skip alpha for int4.
bool WriteBmp_Ints(const char *fn, int* pRawData, int W, int H, int nChannel)
{
	if((!pRawData) || ((nChannel != 3) && (nChannel != 4))	)
	{
		assert(0);
		return false;
	}

	//for test load pfm
	CImage img;
	if(!img.Create(W, H, 24))
	{
		assert(0);
		return false;
	}
	BYTE* pBits = static_cast<BYTE*>(img.GetBits());
	int p = img.GetPitch(); //pitch sometimes is negative
	int* pV = pRawData;
	for (int j = 0; j < H; ++j)
	{
		for (int i = 0; i < W; ++i)
		{
			BYTE* pixAddr = pBits + p * j + i * img.GetBPP() / 8;	//GetBPP: The number of bits per pixel.
			//access pix here. Byte sequence is BGR for 24BPP image.
			*(pixAddr + 2) = (byte)*(pV	   );	//r
			*(pixAddr + 1) = (byte)*(pV + 1);	//g
			*(pixAddr    ) = (byte)*(pV + 2);	//b
			pV += nChannel;
		}
	}

	CString fn1 = (CString)(LPSTR)fn;
	img.Save(fn1 + ".bmp");
	img.Destroy();
	return true;
}

bool IntersectTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, 
					   FLOAT* t, FLOAT* u, FLOAT* v, bool& bAtFront)
{
	// Find vectors for two edges sharing vert0
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	D3DXVECTOR3 pvec;
	D3DXVec3Cross( &pvec, &dir, &edge2 );

	// If determinant is near zero, ray lies in plane of triangle
	FLOAT det = D3DXVec3Dot( &edge1, &pvec );

	D3DXVECTOR3 tvec;
	if( det > 0 )
	{
		bAtFront = true;
		tvec = orig - v0;
	}
	else
	{
		bAtFront = false; //intersect at the back of the triangle
		tvec = v0 - orig;
		det = -det;
	}

	if( det < 0.0001f )
		return FALSE;

	// Calculate U parameter and test bounds
	*u = D3DXVec3Dot( &tvec, &pvec );
	if( *u < 0.0f || *u > det )
		return FALSE;

	// Prepare to test V parameter
	D3DXVECTOR3 qvec;
	D3DXVec3Cross( &qvec, &tvec, &edge1 );

	// Calculate V parameter and test bounds
	*v = D3DXVec3Dot( &dir, &qvec );
	if( *v < 0.0f || *u + *v > det )
		return FALSE;

	// Calculate t, scale parameters, ray intersects triangle
	*t = D3DXVec3Dot( &edge2, &qvec );
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}

unsigned int uintFloorLog(unsigned int base, unsigned int num)
{
	unsigned int result = 0;
	for(unsigned int temp = 0; temp <= num; temp *= base)
		result++;
	return result;
}

unsigned int uintCeilingLog(unsigned int base, unsigned int num)
{
	unsigned int result = 0;
	for(unsigned int temp = 1; temp < num; temp *= base)
		result++;
	return result;
}

unsigned int uintPower(unsigned int base, unsigned int pow)
{
	unsigned int result = 1;
	for(; pow; pow--)
		result *= base;
	return result;
}

unsigned int uintCeilingDiv(unsigned int dividend, unsigned int divisor)
{
	return (dividend + divisor - 1) / divisor;
}

unsigned int tmpCeilingLog(unsigned int base, unsigned int num)
{
	unsigned int cur = 1;
	unsigned int result = 0;
	while(cur < num) {
		cur *= base;
		result++;
	}
	return result;
}
unsigned int tmpPow(unsigned int base, unsigned int p)
{
	unsigned int result = 1;
	for(unsigned int i = 0; i < p; i++)
		result *= base;
	return result;
}
int tmpTranslate(unsigned int treeIdx, unsigned int TS, unsigned int BS, unsigned int AS)
{
	if(treeIdx >= TS)
		return TS-1-BS;

	if(treeIdx >= BS)
		return treeIdx-BS;

	if(treeIdx >= AS)
		return treeIdx-AS + TS - BS;

	assert(false);
	return 0;
}

void OlapSchema::LoadData(char* fn, int nRec)
{
	FILE* file;
	if((file = fopen(fn, "rt")) == NULL)
	{
		printf("failed open file.\n ");
		exit(0);
	}
	nRec = nRec;
	pDim = new DimAttr[nRec];
	fclose(file);
}

void OlapSchema::OnShowAtUpdated(int id)
{
	if(showAt[id] >= _nDim || showAt[id] < 0) //null
	{
		showAt[id] = -1;
		nScaleCur[id] = -1; //this dimension is not displayed: points have coord = 0 at this dim
		scaleRatioCur[id] = -1;
	}
	else
	{
		nScaleCur[id] = nScales[showAt[id]][curLevels[showAt[id]]];
		scaleRatioCur[id] = nScales[showAt[id]][nLevels[showAt[id]] - 1] / nScaleCur[id];
		diceS[id] = 0;
		diceE[id] = nScaleCur[id];
	}
}

void OlapSchema::MoveDicePlane(int planeId, int delta)
{
	if(planeId < 0)
		return;
	int id = planeId / 2;
	if(planeId & 0x1) //1, 3, 5: diceE
	{
		diceE[id] = max(diceS[id], min(diceE[id] + delta, nScaleCur[id]));
	}
	else
	{
		diceS[id] = min(diceE[id], max(diceS[id] + delta, 0));
	}
}
