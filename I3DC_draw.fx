// I3DC.fx, GPUQP, HKUST, yk.cadcg@gmail.com


/////////////////////////////////////////////////////////////////// blend states
BlendState DisableBlend
{
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState BlendAdd
{
    BlendEnable[0] = TRUE;
	SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ONE; //important for AVG
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};
BlendState BlendMin
{
    BlendEnable[0] = TRUE;
	SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = MIN;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = MIN;
    RenderTargetWriteMask[0] = 0x0F;
};
BlendState BlendMax
{
    BlendEnable[0] = TRUE;
	SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = MAX;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = MAX;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState BlendAlpha
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};
/////////////////////////////////////////////////////////////////// DS states

DepthStencilState DLessS1
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS;

	StencilEnable = TRUE;
    StencilReadMask = 0xFFFFFFFF;
	StencilWriteMask = 0;
	
	BackFaceStencilFunc = GREATER;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Keep;
    BackFaceStencilFail = Keep;

	FrontFaceStencilFunc = GREATER;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFail = Keep;
};

DepthStencilState DLessDisableS
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS;

	StencilEnable = FALSE;
	StencilReadMask = 0;
	StencilWriteMask = 0;
	BackFaceStencilFunc = ALWAYS;
	FrontFaceStencilFunc = ALWAYS;
};

DepthStencilState DAlwaysS1
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL; //no matter which, always in front/back of grid??
 	DepthFunc = ALWAYS;
	
	StencilEnable = TRUE;
    StencilReadMask = 0xFFFFFFFF;
	StencilWriteMask = 0;

	BackFaceStencilFunc = GREATER;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Keep;
    BackFaceStencilFail = Keep;
    
    FrontFaceStencilFunc = GREATER;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFail = Keep;
};

DepthStencilState DisableDS
{
    DepthEnable = FALSE;
    DepthWriteMask = ALL;//ZERO;
 	DepthFunc = LESS;

	StencilEnable = FALSE;
	StencilReadMask = 0;
	StencilWriteMask = 0;
	BackFaceStencilFunc = ALWAYS;
	FrontFaceStencilFunc = ALWAYS;
};

DepthStencilState DNoWriteS1
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
 	DepthFunc = ALWAYS;

	StencilEnable = TRUE;
    StencilReadMask = 0xFFFFFFFF;
	StencilWriteMask = 0;
	
	BackFaceStencilFunc = GREATER;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Keep;
    BackFaceStencilFail = Keep;

	FrontFaceStencilFunc = GREATER;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFail = Keep;
};
/////////////////////////////////////////////////////////////////// raster states

RasterizerState EnableCulling
{
    CullMode = BACK;
};
RasterizerState DisableCulling
{
    CullMode = NONE;
};

RasterizerState AALine
{
	AntialiasedLineEnable = TRUE;
};

RasterizerState WireFrameState
{
	FillMode = WireFrame;
};

/////////////////////////////////////////////////////////////////// sampler states

SamplerState sampLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};
SamplerState sampPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
/////////////////////////////////////////////////////////////////// cbuffer
cbuffer cb1
{
    matrix g_mWorldViewProj;
};
cbuffer cbImmutable
{
    float2 g_texcoords[4] = {float2(0,1), float2(1,1), float2(0,0), float2(1,0)};
};

/////////////////////////////////////////////////////////////////// structs
struct POS4
{
    float4 pos : POSITION;
};
struct POS4V
{
    float4 pos : POSITION;
	bool v: foov; //if this point is shown
};
struct SV_POS4
{
    float4 pos: SV_Position;
};
struct POS4MeaV
{
    float4 pos : POSITION;
	int mea: foomea;
	bool v: foov; //if this point is shown
};
struct SV_POS4MEA
{
    float4 pos: SV_Position;
	int mea: foomea;
};

struct SV_POS4TEX2
{
    float4 pos: SV_Position;
	float2 tex: TEXTURE0;
};
struct SV_POS4TEX2MEA
{
    float4 pos: SV_Position;
	float2 tex: TEXTURE0;
	int mea : foomea;
};
struct SOType
{
	int4 pickedR : Fooo1;
	int4 pickedS : Fooo2;
	int sostamp : Fooo3;
};
struct Rec4
{
	int4 rec1: REC4;
};

/////////////////////////////////////////////////////////////////// draw a scatterplot
const float3 v_orig;
const float3 v_gridScale;
void vsAxes(in float3 vert: POSITION,
					in float3 vcol: COLOR,
					out float4 pos: SV_Position,
					out float4 col: COLOR0)
{
	pos = mul(float4(vert * v_gridScale + v_orig, 1), g_mWorldViewProj);
	col = float4(vcol, 1);
}
void psAxes(in float4 pos: SV_Position,
					  in float4 col: COLOR0,
					  out float4 res: SV_Target)
{
	res = col;
}
float4 gridCol = float4(0.3, 0.3, 0.3, 1);
void vsGrids(in float3 vert: POSITION,
			out float4 pos: SV_Position,
			out float4 col: COLOR0)
{
	pos = mul(float4(vert * v_gridScale + v_orig, 1), g_mWorldViewProj);
	col = gridCol;
}
void psGrids(in float4 pos: SV_Position,
					  in float4 col: COLOR0,
					  out float4 res: SV_Target)
{
	res = col;
}

//pass 1: points
Texture2D <int4> texTableDim;
Texture2D <int4> texTableMea;
const uint texTableW;
const int v_sampleRate;
const uint3 v_scaleRatio;
const float3 v_coordScale; //=coordScale / nScaleCur
const int4 v_xyzmAt;
const float4 c_ballCol = float4(0.1, 0.3, 0.5, 1);
SV_POS4 vsPoints(in uint vertexId :SV_VertexID) //for speed, no measure for color
{
	POS4 drawout;
	vertexId *= v_sampleRate;
	uint4 recDim = texTableDim.Load(uint4(vertexId % texTableW, vertexId / texTableW, 0, 0));
	//if any component of v_scaleRatio is -1 (this dim not shown), then uint(-1) = UINT_MAX, and vert=orig at this dim.
	float3 vert = v_orig + (uint3(recDim[v_xyzmAt.x], recDim[v_xyzmAt.y], recDim[v_xyzmAt.z]) / v_scaleRatio) * v_coordScale;
	drawout.pos = mul(float4(vert,1), g_mWorldViewProj );
	return drawout;
}
const float v_lumi;
void psPoints(SV_POS4 input,
			  out float4 res: SV_Target)
{
	res = c_ballCol;
}

//pass 2: gs sprites
const int3 diceS, diceE;
POS4MeaV vsPointsSprite(in uint vertexId :SV_VertexID)
{
	POS4MeaV drawout;
	vertexId *= v_sampleRate;
	uint4 recDim = uint4(vertexId % texTableW, vertexId / texTableW, 0, 0);
	int4 recMea = texTableMea.Load(recDim);
	recDim = texTableDim.Load(recDim);
	int3 curDim = int3(recDim[v_xyzmAt.x], recDim[v_xyzmAt.y], recDim[v_xyzmAt.z]) / v_scaleRatio;
	drawout.v = (all(curDim >= diceS) && all(curDim <= diceE));
	float3 vert = v_orig + curDim  * v_coordScale;
	drawout.pos = mul(float4(vert,1), g_mWorldViewProj );
	drawout.mea = recMea[v_xyzmAt.w];  //todo: easy to extend to multiple measures
	return drawout;
}
const matrix v_matSPP;
[maxvertexcount(4)]
void gsPointsSprite(point POS4MeaV input[1], inout TriangleStream<SV_POS4TEX2MEA> SpriteStream)
{
	if(!input[0].v)
		return;
    SV_POS4TEX2MEA output;
	[unroll(4)]
	for(int i = 0; i < 4; i++)
    {
		output.pos = v_matSPP[i] + input[0].pos;
		output.tex = g_texcoords[i];
		output.mea = input[0].mea;
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();
}
Texture2D v_texParticle;
void psPointsSprite(SV_POS4TEX2MEA input,
			  out float4 res: SV_Target)
{
	res = v_texParticle.Sample(sampLinear, input.tex);
	if (res.a < 0.3)//alpha test
		discard;
	res *= input.mea * v_lumi * c_ballCol;
	res.a = 1; //for Avg blend
}
void psPointsSpriteB(SV_POS4TEX2MEA input,
			  out float4 res: SV_Target)
{
	res = input.mea;
	res.a = 1; //for Avg blend
}
//set mea as 1
POS4V vsPointsSpriteCnt(in uint vertexId :SV_VertexID)
{
	POS4V drawout;
	vertexId *= v_sampleRate;
	uint4 recDim = uint4(vertexId % texTableW, vertexId / texTableW, 0, 0);
	recDim = texTableDim.Load(recDim);
	int3 curDim = int3(recDim[v_xyzmAt.x], recDim[v_xyzmAt.y], recDim[v_xyzmAt.z]) / v_scaleRatio;
	drawout.v = (all(curDim >= diceS) && all(curDim < diceE));
	float3 vert = v_orig + curDim * v_coordScale;
	drawout.pos = mul(float4(vert,1), g_mWorldViewProj );
	return drawout;
}
[maxvertexcount(4)]
void gsPointsSpriteCnt(point POS4V input[1], inout TriangleStream<SV_POS4TEX2> SpriteStream)
{
	if(!input[0].v)
		return;
    SV_POS4TEX2 output;
	[unroll(4)]
	for(int i=0; i<4; i++)
    {
		output.pos = v_matSPP[i] + input[0].pos;
		output.tex = g_texcoords[i];
		SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();
}
void psPointsSpriteCnt(SV_POS4TEX2 input,
			  out float4 res: SV_Target)
{
	res = v_texParticle.Sample(sampLinear, input.tex);
	if (res.a < 0.3)//alpha test
		discard;
	res *= v_lumi * c_ballCol; //*1!
}

const float4 maxCol = float4(1<<30, 1<<30, 1<<30, 1<<30);
void psPointsSpriteFloodMax(SV_POS4TEX2 input,
			  out float4 res: SV_Target)
{
	res = v_texParticle.Sample(sampLinear, input.tex);
	if (res.a < 0.3)//alpha test
		discard;
	res = maxCol;
}
void psPointsSpriteFloodMaxB(SV_POS4TEX2 input,
			  out float4 res: SV_Target)
{
	res = maxCol;
}
Texture2D <float4> texTmp;
void vsPointsSpriteAvg(in float3 vert: POSITION,
					   in float2 tex: TEXCOORD0,
					   out float4 pos: SV_POSITION,
					   out float2 texps: TEXCOORD0)
{
	pos = float4(vert, 1);
	texps = tex;
}
void psPointsSpriteAvg(in float4 pos: SV_POSITION,
					   in float2 tex: TEXCOORD0,
					   out float4 res: SV_Target)
{   
	res = texTmp.Sample(sampPoint, tex);
	if(res.a > 0.9) //>= 1: has count
		res = float4(res.rgb / res.a, 1);
}

float3 v_dicePlaneOff; //dice plane's offset
void vsDicePlane(in float3 vert: POSITION,
					in float3 vcol: COLOR,
					out float4 pos: SV_Position,
					out float4 col: COLOR0)
{
	pos = mul(float4(vert + v_orig + v_dicePlaneOff, 1), g_mWorldViewProj);
	col = float4(vcol, 0.2); //alpha blend
}
void psDicePlane(in float4 pos: SV_Position,
					  in float4 col: COLOR0,
					  out float4 res: SV_Target)
{
	res = col;
}


technique10 techScatterPlot
{
	pass points //only in context
    {
        SetVertexShader(CompileShader(vs_4_0, vsPoints()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psPoints()));
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		SetRasterizerState(DisableCulling); 
		SetDepthStencilState(DLessDisableS, 0);
	}
	pass spritesSum //only in focus
	{
		//SetVertexShader(CompileShader(vs_4_0, vsPoints()));
		//SetGeometryShader(NULL);
		//SetPixelShader(CompileShader(ps_4_0, psPoints()));
		
		SetVertexShader(CompileShader(vs_4_0, vsPointsSprite()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSprite()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSprite()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
	}
	pass spritesCnt //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSpriteCnt()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSpriteCnt()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteCnt()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF ); //Add 1
		SetDepthStencilState(DAlwaysS1, 1);
	}
	pass spritesAvg //only in focus		avg = sum/cnt, directly operate on a texture gened by spritesSumCnt
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSpriteAvg()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteAvg()));
		SetRasterizerState(DisableCulling);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DNoWriteS1, 1);
	}
	pass spritesMax //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSprite()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSprite()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSprite()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendMax, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
	}
	pass spritesMinFloodMax //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSpriteCnt())); //reuse cnt which doesn't sample Mea
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSpriteCnt()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteFloodMax()));
        
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );//noblend, but write
		SetDepthStencilState(DNoWriteS1, 1); //D no test or write 
	}
	pass spritesMin //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSprite()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSprite()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSprite()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendMin, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
	}
	pass grids //only in focus
    {
        SetVertexShader( CompileShader( vs_4_0, vsGrids() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psGrids() ) );
		
		SetRasterizerState(AALine);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		SetDepthStencilState(DLessS1, 1); //not shown in context
	} 
	pass axesCon //only in context
    {
        SetVertexShader( CompileShader( vs_4_0, vsAxes() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psAxes() ) );

		SetRasterizerState(AALine);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DLessDisableS, 0);
   } 
	pass axesFoc //only in focus
	{
		SetVertexShader( CompileShader( vs_4_0, vsAxes() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, psAxes() ) );

		SetRasterizerState(AALine);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DLessS1, 1);
	} 

	/////////////////////////////////////////////////////////////////// billboard, untextured quads
	pass spritesSumB //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSprite()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSprite()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteB()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
	}
	pass spritesMaxB //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSprite()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSprite()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteB()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendMax, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
	}
	pass spritesMinFloodMaxB //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSpriteCnt())); //reuse cnt which doesn't sample Mea
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSpriteCnt()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteFloodMaxB()));
        
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );//noblend, but write
		SetDepthStencilState(DNoWriteS1, 1); //D no test or write 
	}
	pass spritesMinB //only in focus
	{
		SetVertexShader(CompileShader(vs_4_0, vsPointsSprite()));
		SetGeometryShader(CompileShader( gs_4_0, gsPointsSprite()));
		SetPixelShader(CompileShader(ps_4_0, psPointsSpriteB()));
        
		SetRasterizerState(DisableCulling);
		SetBlendState(BlendMin, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
	}

	/////////////////////////////////////////////////////////////////dice
	pass dicePlane //only in focus
    {
        SetVertexShader( CompileShader( vs_4_0, vsDicePlane() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psDicePlane() ) );

		SetRasterizerState(DisableCulling);
		SetBlendState(BlendAlpha, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState(DAlwaysS1, 1);
   }
}

/////////////////////////////////////////////////////////////////// draw env
const float4 c_envCol = float4(0.3, 0.2, 0.2, 1); //float4(0.03, 0.02, 0.03, 1);
void vsEnv(in float3 vert: POSITION,
			out float4 pos: SV_Position)
{
	pos = mul(float4(v_orig + vert * 10, 1), g_mWorldViewProj); //ykdeb: size = 10
}
void psEnv(in float4 pos: SV_Position,
			out float4 res: SV_Target)
{   
	 res = c_envCol;
}
		
technique10 techEnv
{
	pass context
    {
        SetVertexShader(CompileShader( vs_4_0, vsEnv() ) );
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader( ps_4_0, psEnv() ) );
		SetRasterizerState(DisableCulling);
		SetDepthStencilState(DLessDisableS, 0);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	} 
	pass focus
    {
        SetVertexShader(CompileShader( vs_4_0, vsEnv() ) );
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader( ps_4_0, psEnv() ) );
		SetRasterizerState(DisableCulling);
		SetDepthStencilState(DLessS1, 1);
		SetBlendState(BlendAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	} 
}

const float3 v_pickPos, v_origR;
const float v_brushRad; //brush radius
/////////////////////////////////////////////////////////////////// draw context quad
void vsQuad(in float3 vert: POSITION,
			out float4 pos: SV_POSITION)
{
	pos = float4(vert, 1);
}
void psQuad(in float4 pos: SV_POSITION,
			out float4 res: SV_Target)
{   
	 res = float4(1, 0, 0, 0);
}
technique10 techContextQuad
{
	pass p0
    {
        SetVertexShader(CompileShader( vs_4_0, vsQuad() ) );
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader( ps_4_0, psQuad() ) );
	} 
}

/////////////////////////////////////////////////////////////////// draw context boarder
//context boarder
void vsConBorder(in float3 vert: POSITION,
			out float4 pos: SV_POSITION)
{
	pos = float4(vert.xy, 0, 1);
}

void psConBorder(in float4 pos: SV_POSITION,
			out float4 res: SV_Target)
{   
	 res = float4(0.3, 0.6, 0.9, 1);
}
technique10 techContextBorder
{
	pass p0
    {
        SetVertexShader(CompileShader( vs_4_0, vsConBorder() ) );
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader( ps_4_0, psConBorder() ) );
		SetRasterizerState(AALine);
		SetRasterizerState(DisableCulling);
		SetDepthStencilState(DLessDisableS, 0);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	} 
}
//little focus
const float v_zn;
void vsLittleFocus(in float3 vert: POSITION,
			out float4 pos: SV_POSITION)
{
	pos = float4(vert.xy, 1/1.1, 1);
	pos = mul(pos, g_mWorldViewProj);
}
void psLittleFocus(in float4 pos: SV_POSITION,
			out float4 res: SV_Target)
{   
	 res = float4(1, 1, 1, 1);
}
technique10 techLittleFocus
{
	pass p0
    {
        SetVertexShader(CompileShader( vs_4_0, vsLittleFocus() ) );
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader( ps_4_0, psLittleFocus() ) );
		SetRasterizerState(AALine);
		SetDepthStencilState(DisableDS, 0);
	} 
}
/*
/////////////////////////////////////////////////////////////////// retrieve picked
const uint v_SOCounter;
const int texRW;
Rec4 vsRetrievePicked(in uint vid: SV_VertexID,
				   in uint rid: RID)
{
	Rec4 myout;
	if(rid < 0) //bufRS's latter invalid parts, or vid is filtered
	{
		myout.rec1 = int4(-1,-1,-1,-1);
		return myout;
	}
	int4 rec;
	float3 vert;
	rec = texTableDim.Load(uint4(rid % texRW, rid / texRW, 0, 0));
	vert = v_origR;
	vert += float3(rec.gba); //3d object space
	myout.rec1 = int4(rec.x, -1, rec.z, rec.w); //an invalid val for unpicked
	
	if((vid & 0x1) == v_intersectedTable) //this table is picked
		if(distance(vert, v_pickPos) < v_brushRad)
		{
			myout.rec1.y = rec.y; //valid val for picked
		}
	return myout;
}
[maxvertexcount(1)]
void gsRetrievePicked(line Rec4 input[2], inout PointStream<SOType> outputs)
{
	if((input[0].rec1.y >= 0) || (input[1].rec1.y >= 0)) //either endpoint being picked, this line is brushed
	{
		SOType tmp;
		tmp.pickedR = input[0].rec1;
		tmp.pickedS = input[1].rec1;
		tmp.pickedR.y = tmp.pickedS.y = max(input[0].rec1.y, input[1].rec1.y);
		tmp.sostamp = v_SOCounter;
		outputs.Append(tmp);
	}
}
GeometryShader gsRetrievePickedSO = ConstructGSWithSO(CompileShader(gs_4_0, gsRetrievePicked()), "Fooo1.xyzw; Fooo2.xyzw; Fooo3.x");
technique10 techRetrievePicked
{
    pass P0 //pick line
    {
        SetVertexShader(CompileShader(vs_4_0, vsRetrievePicked()));
        SetGeometryShader(gsRetrievePickedSO);
        SetPixelShader(NULL);
		SetDepthStencilState(DisableDS, 0);
    }
}
*/