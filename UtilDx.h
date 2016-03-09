
//(c) Ke Yang, yk.cadcg@gmail.com


#ifndef _UtilDx_H_
#define _UtilDx_H_

#include "Util.h"

extern ID3D10Query*	g_pQuery;

//////////////////////////////////////////////////////////////////////////defines

struct Pos3fCol3f
{
	D3DXVECTOR3 Pos;  
	D3DXVECTOR3 Color;
};
const D3D10_INPUT_ELEMENT_DESC DESC_POS3COL3[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
struct Pos3f
{
	D3DXVECTOR3 Pos;  
};
struct Pos3fTex2f
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

const D3D10_INPUT_ELEMENT_DESC DESC_POS3[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
const D3D10_INPUT_ELEMENT_DESC DESC_POS3TEX2[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }, 
};
const D3D10_INPUT_ELEMENT_DESC DESC_REC[] =
{
	{ "REC", 0, DXGI_FORMAT_R32G32_UINT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
const D3D10_INPUT_ELEMENT_DESC DESC_RID[] =
{
	{ "RID", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

struct PickedElement
{
	D3DXVECTOR4 pix;
	POINT pos;
};

class DXOlapTable
{
public:
	DXOlapTable(OlapSchema* pS, ID3D10Device* pd3dDevice); //only for 3
	~DXOlapTable(){SAFE_RELEASE(pTexDimData); SAFE_RELEASE(pSRVDimData);SAFE_RELEASE(pTexMeaData); SAFE_RELEASE(pSRVMeaData);};
	//build CSS-tree on pTree
	void NewIndex(int buildOn, ID3D10Device* pd3dDevice, ID3D10Effect* pEffect);
	
	int nRec;
	int nTexPerRec; //= num of ints in Rec / 4
	int nTexel; 
	int texDataW, texDataH;
	ID3D10Texture2D* pTexDimData;
	ID3D10ShaderResourceView* pSRVDimData;
	ID3D10Texture2D* pTexMeaData;
	ID3D10ShaderResourceView* pSRVMeaData;
protected:
private:
};

class DXScatterPlot
{
public:	
	DXScatterPlot();
	~DXScatterPlot();
	D3DXVECTOR3		orig;	
	Pos3fCol3f*		pAxesVert; //6 verts for 3 axes
	ID3D10Buffer*   pAxesVB;	
	Pos3f*		pGridsVert[3];//ab+ac+bc lines, where a,b,c are nScales of current level on each axis
	ID3D10Buffer*   pGridsVB[3];
	
	//init axes of a unit-sized plot centered at (0,0,0)
	void DXInitAxisVerts(D3DXVECTOR3 coordScale);
	
	void SetPlotOrig(D3DXVECTOR3 coordScale);

	//axisId: recalculate pPlot's gridVerts and gridVB on this axis, 
	//sc: num scales on this axis
	void UpdateGrid(int axisId, OlapSchema* pSchema, ID3D10Device* pd3dDevice); //update pGridsVerts and pGridsVB

	void UpdateDice(int axisId, OlapSchema* pSchema);

	//dice
	//new 3 Pos3Col3 VBs for dice planes, perpendicular to x, y, z axis, respectively. 
	void NewDicePlaneVBs(ID3D10Device* pd3dDevice);
	
	ID3D10Buffer* pDicePlaneVB[3]; //vertices of the plane perpendicular to x, y, z axis, respectively
	D3DXVECTOR3 dicePlaneOff[6]; //dice plane's start/end offset: {xs,xe,ys,ye,zs,ze}
	int diceActivePlaneId;  //-1: can't drag plane. 0-6: can drag one of {xs,xe,ys,ye,zs,ze} plane
	//void MoveDicePlane(int nMouseWheel, OlapSchema* pSchema);
	D3DXVECTOR3 coordScale;
protected:
private:
};
//////////////////////////////////////////////////////////////////////////funcs

//compile effect file
//output: ppEffect
//input: other para
void DXNewEffect(ID3D10Effect** ppEffect, ID3D10Device* pd3dDevice, LPCTSTR pFileName);

//size of fmt in bytes.
int SizeOf(const DXGI_FORMAT fmt);

//New tex for gpgpu use. caller should release it by himself.
//Output: descTex, ppTex
//Input: other paras. if pSysMem !=NULL, then init, o/w don't.
void DXNewTex(D3D10_TEXTURE2D_DESC& descTex, ID3D10Texture2D** ppTex, int W, int H, DXGI_FORMAT fmt, D3D10_USAGE usg, UINT bind, UINT cpuAccess, void *pSysMem, ID3D10Device* pd3dDevice);

//new a SR view for a tex with given desc
//output: ppSRViewTex
//input: other para
void DXNewTexSRView(ID3D10ShaderResourceView** ppSRViewTex, ID3D10Device* pd3dDevice, ID3D10Texture2D* pTexData, D3D10_TEXTURE2D_DESC descTex);

//new a RT view for a tex with given desc
//output: ppSRViewTex
//input: other para
void DXNewTexRTView(ID3D10RenderTargetView** ppRTViewTex, ID3D10Device* pd3dDevice, ID3D10Texture2D* pTexData, D3D10_TEXTURE2D_DESC descTex);

//set viewport
//output: none.
void DXViewPort(ID3D10Device* pd3dDevice, const int W, const int H);

//draw nV verts.
//output: null.
void DXDraw(ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique, const int nV);

//sequentially read nR fmt elments from fmt tex of WxH to pR, meantime discard Tex 
//Limitations: 
//	1, Tex and pR should be int32/float32-based. 
//	2, read Tex from start, no filter.
//	3, alloc stage texture within, no reusing.
template<class T>
void DXReadTex(T* pDst, ID3D10Texture2D* pTexData, int W, int H, int nR, DXGI_FORMAT fmt, ID3D10Device* pd3dDevice)
{
	ID3D10Texture2D* g_pTexStage1Pix = NULL;
	D3D10_TEXTURE2D_DESC descTex;
	DXNewTex(descTex, &g_pTexStage1Pix, W, H, fmt, D3D10_USAGE_STAGING, 0, D3D10_CPU_ACCESS_READ, NULL, pd3dDevice);
	pd3dDevice->CopyResource(g_pTexStage1Pix, pTexData);

	D3D10_MAPPED_TEXTURE2D mappedTex;
	g_pTexStage1Pix->Map(0, D3D10_MAP_READ, NULL, &mappedTex);

	int pitchEles = mappedTex.RowPitch / sizeof(T);			//num of eles in each row in src
	int vPitchEles = descTex.Width * SizeOf(fmt) / sizeof(T);	//num of valid eles in each row
	T* pSrc = (T*)mappedTex.pData;
	T* pR = pDst;
	for(int row = 0; row < (int)descTex.Height; ++row)
	{
		if((row + 1) * descTex.Width <= nR)	//adding this line would not surpass nR - 1
		{
			memcpy(pR, pSrc, vPitchEles * sizeof(T));
			pR += vPitchEles;
			pSrc += pitchEles;
		}
		else	//last incomplete line
		{
			memcpy(pR, pSrc, (nR - row * descTex.Width) * SizeOf(fmt));
			break;
		}
	}
	g_pTexStage1Pix->Unmap(0);
	SAFE_RELEASE(g_pTexStage1Pix);
}

//////////////////////////////////////////////////////////////////////////Timer
//refer to http://msdn2.microsoft.com/en-us/library/bb172234.aspx , "Accurately Profiling Direct3D API Calls"
//learned from NVidia CUDA cutil's StopWatch class: NVIDIA Corporation\NVIDIA CUDA SDK\common\inc\stopwatch_win.h

class DXTimer
{
public:
	DXTimer()
	{
		m_totalTime = 0.f;
		m_bRun = false;
		//set m_freq
		LARGE_INTEGER temp;
		QueryPerformanceFrequency((LARGE_INTEGER*) &temp);
		m_freq = ((double) temp.QuadPart) / 1000.0;
	}
	~DXTimer(){}

	//start timing
	void Start()
	{
		QueryPerformanceCounter((LARGE_INTEGER*) &m_startTime);
		m_bRun = true;
	}

	//stop timing: accumulate total time
	void Stop()
	{
		QueryPerformanceCounter((LARGE_INTEGER*) &m_endTime);
		m_totalTime += (float) (((double) m_endTime.QuadPart - (double) m_startTime.QuadPart) / m_freq);
		m_bRun = false;
	}

	//reset time counter to 0
	void Reset()
	{
		m_totalTime = 0;
		if( m_bRun )
			QueryPerformanceCounter((LARGE_INTEGER*) &m_startTime);
	}

	//total time
	float GetTime()
	{
		float retval = m_totalTime;
		if(m_bRun)		//if has called a Start() but no last Stop() call, then add (current time - last Start() call)
		{
			LARGE_INTEGER temp;
			QueryPerformanceCounter((LARGE_INTEGER*) &temp);
			retval += (float) (((double) (temp.QuadPart - m_startTime.QuadPart)) / m_freq);
		}
		return retval;
	}

protected:
private:
	LARGE_INTEGER  m_startTime;
	LARGE_INTEGER  m_endTime;
	float  m_totalTime;//total m_diffTime
	bool m_bRun;	//timer is running
	double  m_freq;	//tick frequency
};

//////////////////////////////////////////////////////////////////////////

void DXNewQuadVBPos3(ID3D10Buffer** ppVB, ID3D10Device* pd3dDevice);
void DXNewQuadVBPos3Tex2(ID3D10Buffer** ppVB, ID3D10Device* pd3dDevice);
void DXNewCubeVB(ID3D10Buffer** ppVB, ID3D10Buffer** ppVBInd, ID3D10Device* pd3dDevice, D3DXVECTOR3 coordScale);

//create vertex buffer
//output: ppVB
//input: other para
template<class T>
void DXNewVB(ID3D10Buffer** ppVB, ID3D10Device* pd3dDevice, T* vertices, const int nV)
{
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(T) * nV;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA initialData1;
	initialData1.pSysMem = vertices;
	V(pd3dDevice->CreateBuffer(&bd, &initialData1, ppVB));
}

template<class T>
void DXNewQuadVBIndexed(ID3D10Buffer** ppVB, ID3D10Buffer** ppVBInd, ID3D10Device* pd3dDevice, T* vertices, const int nV, DWORD* indices, const int nI)
{
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(T) * nV;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	V(pd3dDevice->CreateBuffer(&bd, &InitData, ppVB));
	// Set vertex buffer
	UINT stride = sizeof(T);
	UINT offset = 0;
	pd3dDevice->IASetVertexBuffers(0, 1, ppVB, &stride, &offset);

	// Create index buffer
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * nI;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = indices;
	V(pd3dDevice->CreateBuffer( &bd, &InitData, ppVBInd));
	pd3dDevice->IASetIndexBuffer(*ppVBInd, DXGI_FORMAT_R32_UINT, 0 );
	pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#endif