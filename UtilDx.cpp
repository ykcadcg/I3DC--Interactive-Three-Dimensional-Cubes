
//(c) Ke Yang, yk.cadcg@gmail.com

#include "DXUT.h"
#include "UtilDx.h"

static ID3D10ShaderResourceView *const c_pNullSRV[1] = {NULL};
//////////////////////////////////////////////////////////////////////////funcs

void DXNewEffect(ID3D10Effect** ppEffect, ID3D10Device* pd3dDevice, LPCTSTR pFileName)
{
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
	//dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	//dwShaderFlags |= D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT;
	//dwShaderFlags |= D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT;
#endif	
	ID3D10Blob* err = 0;
	D3DX10CreateEffectFromFile(pFileName, NULL, NULL, "fx_4_0", dwShaderFlags, 0, pd3dDevice, NULL, NULL, ppEffect, &err, NULL);
	if(err)
	{
		cout<<(char*)err->GetBufferPointer();
		SAFE_RELEASE(err);    
		cout<<"failed D3DX10CreateEffectFromFile!"<<endl;
		assert(0);
		return;
	}
	if((!ppEffect) || (!(*ppEffect)))
	{
		cout<<"failed D3DX10CreateEffectFromFile!"<<endl;
		assert(0);
		return;
	}
}

int SizeOf(const DXGI_FORMAT fmt)
{
	if(fmt >= DXGI_FORMAT_R32G32B32A32_TYPELESS &&
		fmt <= DXGI_FORMAT_R32G32B32A32_SINT)
		return 16;
	if(fmt >= DXGI_FORMAT_R32G32B32_TYPELESS &&
		fmt <= DXGI_FORMAT_R32G32B32_SINT)
		return 12;
	if(fmt >= DXGI_FORMAT_R16G16B16A16_TYPELESS &&
		fmt <= DXGI_FORMAT_X32_TYPELESS_G8X24_UINT)
		return 8;
	if(fmt >= DXGI_FORMAT_R10G10B10A2_TYPELESS &&
		fmt <= DXGI_FORMAT_X24_TYPELESS_G8_UINT)
		return 4;

	cout<<"failed Sizeof()!"<<endl;
	assert(0);
	return -1;
}

void DXNewTex(D3D10_TEXTURE2D_DESC& descTex, ID3D10Texture2D** ppTex, int W, int H, DXGI_FORMAT fmt, D3D10_USAGE usg, UINT bind, UINT cpuAccess, void *pSysMem, ID3D10Device* pd3dDevice)
{
	descTex.Width = W;
	descTex.Height = H;
	descTex.Format = fmt;
	descTex.Usage = usg;
	descTex.BindFlags = bind;
	descTex.CPUAccessFlags = cpuAccess;
	//default
	descTex.MipLevels = 1;
	descTex.ArraySize = 1;
	descTex.SampleDesc.Count = 1;
	descTex.SampleDesc.Quality = 0;
	descTex.MiscFlags = 0;

	if(pSysMem)
	{
		D3D10_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = pSysMem;
		initialData.SysMemPitch = SizeOf(fmt) * W;
		V(pd3dDevice->CreateTexture2D(&descTex, &initialData, ppTex));
	}
	else
		V(pd3dDevice->CreateTexture2D(&descTex, NULL, ppTex));
}

void DXNewTexSRView(ID3D10ShaderResourceView** ppSRViewTex, ID3D10Device* pd3dDevice, ID3D10Texture2D* pTexDimData, D3D10_TEXTURE2D_DESC descTex)
{
	D3D10_SHADER_RESOURCE_VIEW_DESC descResView;
	descResView.Format = descTex.Format;
	descResView.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	descResView.Texture2D.MipLevels = 1;
	descResView.Texture2D.MostDetailedMip = 0;
	V(pd3dDevice->CreateShaderResourceView(pTexDimData, &descResView, ppSRViewTex));
}

void DXNewTexRTView(ID3D10RenderTargetView** ppRTViewTex, ID3D10Device* pd3dDevice, ID3D10Texture2D* pTexDimData, D3D10_TEXTURE2D_DESC descTex)
{
	D3D10_RENDER_TARGET_VIEW_DESC descRTView;
	descRTView.Format = descTex.Format;
	descRTView.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	descRTView.Texture2D.MipSlice = 0;
	V(pd3dDevice->CreateRenderTargetView(pTexDimData, &descRTView, ppRTViewTex));
}

void DXNewQuadVBPos3(ID3D10Buffer** ppVB, ID3D10Device* pd3dDevice)
{
	Pos3f vertices[] =
	{
		{D3DXVECTOR3(-1.f, 1.f,	0.5f)},
		{D3DXVECTOR3(1.f, -1.f,	0.5f)},	
		{D3DXVECTOR3(-1.f,-1.f,	0.5f)},	
		{D3DXVECTOR3(-1.f, 1.f,		0.5f)},	
		{D3DXVECTOR3(1.f,	 1.f,	0.5f)},	
		{D3DXVECTOR3(1.f,	-1.f,	0.5f)},	
	};	
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Pos3f) * 6;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA initialData1;
	initialData1.pSysMem = vertices;
	V(pd3dDevice->CreateBuffer(&bd, &initialData1, ppVB));
}

void DXNewQuadVBPos3Tex2(ID3D10Buffer** ppVB, ID3D10Device* pd3dDevice)
{
	Pos3fTex2f vertices[] =
	{
		{D3DXVECTOR3(-1.f, 1.f,	0.5f),		D3DXVECTOR2(0.0f, 0.0f)},
		{D3DXVECTOR3(1.f, -1.f,	0.5f),		D3DXVECTOR2(1.0f, 1.0f)},	
		{D3DXVECTOR3(-1.f,-1.f,	0.5f),		D3DXVECTOR2(0.0f, 1.0f)},	
		{D3DXVECTOR3(-1.f, 1.f,		0.5f),	D3DXVECTOR2(0.0f, 0.0f)},	
		{D3DXVECTOR3(1.f,	 1.f,	0.5f),	D3DXVECTOR2(1.0f, 0.0f)},	
		{D3DXVECTOR3(1.f,	-1.f,	0.5f),	D3DXVECTOR2(1.0f, 1.0f)},	
	};	
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Pos3fTex2f) * 6;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA initialData1;
	initialData1.pSysMem = vertices;
	V(pd3dDevice->CreateBuffer(&bd, &initialData1, ppVB));
}


void DXViewPort(ID3D10Device* pd3dDevice, const int W, const int H)
{
	D3D10_VIEWPORT vp;
	vp.Width = W;
	vp.Height = H;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pd3dDevice->RSSetViewports(1, &vp);
}

void DXDraw(ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique, const int nV)
{
	D3D10_TECHNIQUE_DESC techDesc;
	pTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pTechnique->GetPassByIndex(p)->Apply(0);
		pd3dDevice->Draw(nV, 0);
	}
}

void InitResource_Template()
{
	//////////////////////////////////////////////////////////////////////////effect

	//////////////////////////////////////////////////////////////////////////const

	//////////////////////////////////////////////////////////////////////////input tex

	//////////////////////////////////////////////////////////////////////////output tex

	//////////////////////////////////////////////////////////////////////////output SO bufRS

	//////////////////////////////////////////////////////////////////////////states

	//////////////////////////////////////////////////////////////////////////tech

	////////////////////////////////////////////////////////////////////////////VB

	//////////////////////////////////////////////////////////////////////////viewport

	////////////////////////////////////////////////////////////////////////////matrix
}


void DXScatterPlot::DXInitAxisVerts(D3DXVECTOR3 coordScale)
{
	pAxesVert[0].Pos = pAxesVert[2].Pos = pAxesVert[4].Pos = D3DXVECTOR3(0,0,0);
	pAxesVert[1].Pos = D3DXVECTOR3(1.1, 0, 0) * coordScale;
	pAxesVert[0].Color = pAxesVert[1].Color = D3DXVECTOR3(1, 0, 0) ; //r

	pAxesVert[3].Pos = D3DXVECTOR3(0, 1.1, 0) * coordScale;
	pAxesVert[2].Color = pAxesVert[3].Color = D3DXVECTOR3(0, 1, 0); //g

	pAxesVert[5].Pos = D3DXVECTOR3(0, 0, 1.1) * coordScale;
	pAxesVert[4].Color = pAxesVert[5].Color = D3DXVECTOR3(0, 0, 1); //b
}

//////////////////////////////////////////////////////////////////////////OlapSchema
OlapSchema::OlapSchema()
{
	pDim = NULL;
	pMea = NULL;
	for(int i = 0; i < 3; ++i)
	{
		showAt[i] = i; 
		bDice[i] = false;
	}
	meaAt = 0; 
	bDiceSel = false;
}
//////////////////////////////////////////////////////////////////////////DXOlapTable
DXOlapTable::DXOlapTable(OlapSchema* pS, ID3D10Device* pd3dDevice)
{
	nRec = pS->nRec;
	nTexPerRec = (int)ceilf(sizeof(DimAttr) / (sizeof(int) * 4.f)); 
	nTexel = nRec * nTexPerRec;
	texDataW = (int)powf(2, int(floorf((logf(nTexel) / logf(2)) / 2.f)));
	texDataH = (int)ceilf(float(nTexel) / texDataW);
	D3D10_TEXTURE2D_DESC descTex;
	DXNewTex(descTex, &pTexDimData, texDataW, texDataH, DXGI_FORMAT_R32G32B32A32_SINT, D3D10_USAGE_DEFAULT, D3D10_BIND_SHADER_RESOURCE, 0, pS->pDim, pd3dDevice);
	DXNewTexSRView(&pSRVDimData, pd3dDevice, pTexDimData, descTex);
	DXNewTex(descTex, &pTexMeaData, texDataW, texDataH, DXGI_FORMAT_R32G32B32A32_SINT, D3D10_USAGE_DEFAULT, D3D10_BIND_SHADER_RESOURCE, 0, pS->pMea, pd3dDevice);
	DXNewTexSRView(&pSRVMeaData, pd3dDevice, pTexMeaData, descTex);
}

DXScatterPlot::DXScatterPlot()
{
	pAxesVert = NULL; 
	pAxesVB = NULL; 
	for (int i = 0; i < 3; ++i)
	{
		pGridsVert[i] = NULL; 
		pGridsVB[i] = NULL;
		pDicePlaneVB[i] = NULL;
	}
	diceActivePlaneId = -1;
}

DXScatterPlot::~DXScatterPlot()
{
	SAFE_DELETE(pAxesVert); 
	SAFE_RELEASE(pAxesVB); 
	for (int i = 0; i < 3; ++i)
	{
		SAFE_DELETE(pGridsVert[i]); 
		SAFE_RELEASE(pGridsVB[i]);
		SAFE_RELEASE(pDicePlaneVB[i]);
	}
}

void DXScatterPlot::UpdateGrid(int id, OlapSchema* pSchema, ID3D10Device* pd3dDevice) 
{
	int sc = pSchema->nScaleCur[id];
	//VB of plot
	SAFE_DELETE(pGridsVert[id]);
	SAFE_RELEASE(pGridsVB[id]);
	if(sc <= 0)
		return;
	pGridsVert[id] = new Pos3f[sc * 2 * 2]; //2 set of lines on 2 planes
	float itv = 1.f / sc; //interval
	D3DXVECTOR3 v0, v1, v2; //v0-v1 is a line on on plane, v0-v2 is on another plane
	for(int i = 0; i < sc; ++i)
	{
		switch (id)
		{
		case 0:
			v0 = D3DXVECTOR3(i * itv, -0.01f, -0.01f) * 1.00;
			v1 = D3DXVECTOR3(i * itv, 1.f, -0.01f) * 1.00;
			v2 = D3DXVECTOR3(i * itv, -0.01f, 1.f) * 1.00;
			break;
		case 1:
			v0 = D3DXVECTOR3(-0.01f, i * itv, -0.01f) * 1.00;
			v1 = D3DXVECTOR3(-0.01f, i * itv, 1.f) * 1.00;
			v2 = D3DXVECTOR3(1.f, i * itv, -0.01f) * 1.00;
			break;
		case 2:
			v0 = D3DXVECTOR3(-0.01f, -0.01f, i * itv) * 1.00;
			v1 = D3DXVECTOR3(1.f, -0.01f, i * itv) * 1.00;
			v2 = D3DXVECTOR3(-0.01f, 1.f, i * itv) * 1.00;
			break;
		default:
			assert(0);
		}
		if(i == 0) //do not overlap with axes
		{
			pGridsVert[id][i * 2].Pos = v0; //on plane1
			pGridsVert[id][i * 2 + 1].Pos = v0;
			pGridsVert[id][sc * 2 + i * 2].Pos = v0; //on plane2
			pGridsVert[id][sc * 2 + i * 2 + 1].Pos = v0;
			continue;
		}
		pGridsVert[id][i * 2].Pos = v0; //on plane1
		pGridsVert[id][i * 2 + 1].Pos = v1;
		pGridsVert[id][sc * 2 + i * 2].Pos = v0; //on plane2
		pGridsVert[id][sc * 2 + i * 2 + 1].Pos = v2;
	}
	DXNewVB<Pos3f>(&pGridsVB[id], pd3dDevice, pGridsVert[id], sc * 2 * 2);
}

void DXScatterPlot::UpdateDice(int id, OlapSchema* pSchema) 
{
	switch (id)
	{
	case 0:
		//xs, xe
		dicePlaneOff[0] = D3DXVECTOR3(pSchema->diceS[0] / (float)pSchema->nScaleCur[0], 0, 0) * 1.00;;
		dicePlaneOff[1] = D3DXVECTOR3(pSchema->diceE[0] / (float)pSchema->nScaleCur[0], 0, 0) * 1.00;
		break;
	case 1:
		//ys, ye
		dicePlaneOff[2] = D3DXVECTOR3(0, pSchema->diceS[1] / (float)pSchema->nScaleCur[1], 0) * 1.00;
		dicePlaneOff[3] = D3DXVECTOR3(0, pSchema->diceE[1] / (float)pSchema->nScaleCur[1], 0) * 1.00;
		break;
	case 2:
		//zs, ze
		dicePlaneOff[4] = D3DXVECTOR3(0, 0, pSchema->diceS[2] / (float)pSchema->nScaleCur[2]) * 1.00;
		dicePlaneOff[5] = D3DXVECTOR3(0, 0, pSchema->diceE[2] / (float)pSchema->nScaleCur[2]) * 1.00;
		break;
	default:
		assert(0);
	}
}

void DXScatterPlot::NewDicePlaneVBs(ID3D10Device* pd3dDevice)
{
	D3DXVECTOR3 r(1.1, 0, 0), g(0, 1.1, 0), b(0, 0, 1.1); //colors
	D3DXVECTOR3 o(0, 0, 0), x(1.1, 0, 0), y(0, 1.1, 0), z(0, 0, 1.1), xy(1.1, 1.1, 0), yz(0, 1.1, 1.1), xz(1.1, 0, 1.1);
	
	Pos3fCol3f perp[3][6] = {
		{{y, r}, {yz, r}, {z, r}, {y, r}, {z, r}, {o, r}}, //x
		{{o, g}, {z, g}, {xz, g}, {o, g}, {xz, g}, {x, g}}, //y
		{{xy, b}, {y, b}, {o, b}, {xy, b}, {o, b}, {x, b}}, //y
	};
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 6; ++j)
			perp[i][j].Pos *= coordScale;
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Pos3fCol3f) * 6;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA initialData1;
	for (int i = 0; i < 3; ++i)
	{
		initialData1.pSysMem = perp[i];
		V(pd3dDevice->CreateBuffer(&bd, &initialData1, &pDicePlaneVB[i]));
	}
}
void DXScatterPlot::SetPlotOrig(D3DXVECTOR3 coordScale)
{
	orig = D3DXVECTOR3(-.5f, -.5f, 0) * coordScale; 
}
//
//void DXScatterPlot::MoveDicePlane(int nMouseWheel, OlapSchema* pSchema)
//{
//	if(diceActivePlaneId >= 0)
//	{
//		float* pOff = &dicePlaneOff[diceActivePlaneId][diceActivePlaneId / 2];
//		*pOff += nMouseWheel / (float)pSchema->nScaleCur[diceActivePlaneId / 2];
//		*pOff = min(1.f, max(0.f, *pOff));
//	}
//}

void DXNewCubeVB(ID3D10Buffer** ppVB, ID3D10Buffer** ppIndexBuffer,  ID3D10Device* pd3dDevice, D3DXVECTOR3 coordScale)
{
	Pos3f vertices[] =
	{
		{ D3DXVECTOR3( -0.5f,  0.5f, -0.5f ) * coordScale},
		{ D3DXVECTOR3(  0.5f,  0.5f, -0.5f ) * coordScale},
		{ D3DXVECTOR3(  0.5f,  0.5f,  0.5f ) * coordScale},
		{ D3DXVECTOR3( -0.5f,  0.5f,  0.5f ) * coordScale},
		{ D3DXVECTOR3( -0.5f, -0.5f, -0.5f ) * coordScale},
		{ D3DXVECTOR3(  0.5f, -0.5f, -0.5f ) * coordScale},
		{ D3DXVECTOR3(  0.5f, -0.5f,  0.5f ) * coordScale},
		{ D3DXVECTOR3( -0.5f, -0.5f,  0.5f ) * coordScale},
	};
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Pos3f) * 8;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	V(pd3dDevice->CreateBuffer(&bd, &InitData, ppVB));
	// Create index buffer
	DWORD indices[] = {3,1,0,	2,1,3, 0,5,4,	1,5,0,	3,4,7,	0,4,3,	1,6,5,	2,6,1,	2,7,6,	3,7,2,	6,4,5, 7,4,6};
	//backfaces
	//DWORD indices[] = {3,0,1,	2,3,1, 0,4,5,	1,0,5,	3,7,4,	0,3,4,	1,5,6,	2,1,6,	2,6,7,	3,2,7,	6,5,4, 7,6,4};
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( DWORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = indices;
	ID3D10Buffer* pIndexBuffer = NULL;
	V(pd3dDevice->CreateBuffer( &bd, &InitData, ppIndexBuffer));
}

