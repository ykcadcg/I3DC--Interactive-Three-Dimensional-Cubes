//--------------------------------------------------------------------------------------
// File: I3DC.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTsettingsdlg.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"
#include "resource.h"
#include <fcntl.h> 
#include <conio.h>
#include <io.h>
#include <iostream>

#include "Util.h"
#include "UtilDx.h"
#include "skybox.h"

using namespace std;

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////consts 
//yk
const float cConSizeRatio = 0.2f;
const int c_nVContextBorder = 8;
const bool g_bLoadFile = false;
const int c_nTextsOnAxis = 8; //MAX (not necessarily exactly!) num of text labels on each axis
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
bool						g_bShowHelp = false;     // If true, it renders the UI control text
CFirstPersonCamera			g_Camera;               // A model viewing camera
CModelViewerCamera			g_MCamera;              // Camera for mesh control
CModelViewerCamera			g_LCamera;              // Camera for easy light movement control
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg				g_D3DSettingsDlg;       // Device settings dialog
CDXUTTextHelper*			g_pTxtHelper = NULL;
CDXUTDialog					g_HUD;                  // manages the 3D UI
CDXUTDialog					g_SettingUI;             // dialog for sample specific controls
D3DXMATRIX					g_mWorldScaling;        // Scaling to apply to mesh
D3DXMATRIX					g_mWorldBack[2];        // Background matrix
int							g_nCurrBackground;
bool						g_bLeftButtonDown = false;
bool						g_bRightButtonDown = false;
bool						g_bMiddleButtonDown = false;
//yk
static bool					g_bStartedRender = false;
float						g_conDist = 3.f; //distance of eye from world center in Context
bool						g_bPickSuccess = false;
int							g_brushRad = 1;
int							g_sampleCon = 1;
int							g_sampleFoc = 1;
bool						g_bShowSetting = true;
bool						g_bDrawContext = true;
bool						g_bDrawEnv = true;
float						g_spriteSize = .005f; //.015f;
float						g_lumi = _bDebug ? 1.f : 0.10f; //borrow light lumi for point lumi
D3DXVECTOR3					g_handPos;
bool						g_bDrawGrid = true;
int							g_changedAxis = -1; //scatterplot's X/Y/Z "show" and "level" menu has updated
PickedElement				g_picked;
D3DXVECTOR3					g_coordScale = D3DXVECTOR3(1, 1, 1);

//Direct3D10 resources
ID3DX10Font*                    g_pFont10 = NULL;       // Font for drawing text
ID3DX10Sprite*                  g_pSprite10 = NULL;
ID3D10Effect*                   g_pEffect = NULL;
ID3D10EffectTechnique*          g_pTechDrawPlot = NULL;
ID3D10EffectMatrixVariable*     g_pmWorldViewProj = NULL;
ID3D10EffectShaderResourceVariable*   g_pDiffuseTex = NULL;
//yk
OlapSchema*						g_schema = NULL;
DXOlapTable*					g_pTable = NULL; //drawn by tech's pass1
//scatter plot and axes
DXScatterPlot*					g_pPlot = NULL;
ID3D10InputLayout*              g_pLayoutPos3Col3 = NULL;
ID3D10DepthStencilState*		g_pDSEnableDS = NULL;

//context
ID3D10InputLayout*              g_pContextLayout = NULL; 
ID3D10EffectTechnique*          g_pTechContextQuad = NULL;
ID3D10EffectTechnique*          g_pTechEnv = NULL;
ID3D10EffectTechnique*          g_pTechContextBorder = NULL;
ID3D10EffectTechnique*          g_pTechLittleFocus = NULL;
ID3D10InputLayout*              g_pLayoutPos3 = NULL;
ID3D10Buffer*					g_pContextBorderVB = NULL;
ID3D10Buffer*					g_pLittleFocusVB = NULL;
static ID3D10ShaderResourceView *const c_pNullSRV[1] = {NULL};

//envmap
ID3D10Buffer*					g_pEnvVB = NULL;
ID3D10Buffer*					g_pEnvVBInd = NULL;
CSkybox							g_envmap;

//picked
ID3D10Buffer*					g_pBufPicked = NULL;
ID3D10Buffer*					g_pStagePicked = NULL;
ID3D10ShaderResourceView*		g_pParticleSRV = NULL;

//other resources
ID3D10Buffer*					g_pVBQuadPos3 = NULL;
ID3D10Texture2D*				g_pTexBack = NULL;	//back buffer of viewport, for intermediate tmp storage
ID3D10ShaderResourceView*		g_pSRVBack = NULL;
ID3D10RenderTargetView*			g_pRTVBack = NULL;
ID3D10InputLayout*              g_pLayoutPos3Tex2 = NULL;
ID3D10Buffer*					g_pVBQuadPos3Tex2 = NULL;
ID3D10Texture2D*				g_pTexStage1Pix = NULL;
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_CHANGEDEVICE        4
#define IDC_LUMINANCELABEL      7
#define IDC_LUMINANCE           8
#define IDC_BACKGROUND          9
//yk
#define IDC_ConDistLabel           12
#define IDC_ConDist           13
#define IDC_BrushRadLabel           14
#define IDC_BrushRad           15
#define IDC_LoadTable			16
#define IDC_XShow					17
#define IDC_YShow					18
#define IDC_ZShow					19
#define IDC_SampleConLabel			23
#define IDC_SampleCon				24
#define IDC_SampleFocLabel			25
#define IDC_SampleFoc				26
#define IDC_SpriteSizeLabel			27
#define IDC_SpriteSize				28

#define IDC_CoordScaleXLabel		29
#define IDC_CoordScaleX				30
#define IDC_CoordScaleYLabel		31
#define IDC_CoordScaleY				32
#define IDC_CoordScaleZLabel		33
#define IDC_CoordScaleZ				34

//more GUI
#define IDC_BkgdCol				36
#define IDC_PointCol			37
#define IDC_GridCol				38
#define IDC_LabelCol			39
#define IDC_XShowLabel		40
#define IDC_YShowLabel		41
#define IDC_ZShowLabel		42
#define IDC_XLevel		43
#define IDC_YLevel		44
#define IDC_ZLevel		45
#define IDC_Mea		46
#define IDC_MeaLabel		47
#define IDC_Aggr		48
#define IDC_AggrLabel		49

#define IDC_DiceLabel		50 
#define IDC_DiceX			51 //check
#define IDC_DiceY			52
#define IDC_DiceZ			53
#define IDC_DiceSel		54 //button
#define IDC_DiceOrig	55 //

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
void    CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

bool    CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnD3D10SwapChainResized( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnD3D10SwapChainReleasing( void* pUserContext );
void    CALLBACK OnD3D10DestroyDevice( void* pUserContext );

void    InitApp();
void    DrawText(ID3D10Device* pd3dDevice);
//yk
//init stencil buffer for the context
void	InitContextStencil(ID3D10Device* pd3dDevice);
//draw all available scatterplots
void	DrawPlots(ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTech, DXOlapTable* pTable, DXScatterPlot* pPlot, OlapSchema* pSchema, bool bDrawFocus);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackMouse( MouseProc );
    DXUTSetCallbackFrameMove( OnFrameMove );

    DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
    DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
    DXUTSetCallbackD3D10SwapChainResized( OnD3D10SwapChainResized );
    DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10SwapChainReleasing );
    DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
    DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"I3DC" );

	//////////////////////////////////////////////////////////////////////////init Console
	if (!AllocConsole()) 
		;//MessageBox(0, L"Failed to create the console!", 0, 0); 
	//Redirect output to stdio. 
	int hCrt; 
	FILE *hf; 
	hCrt = _open_osfhandle((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT); 
	hf = _fdopen( hCrt, "w" ); 
	*stdout = *hf; 
	setvbuf( stdout, NULL, _IONBF, 0 ); 
	SetConsoleTitle((LPCWSTR)L"GPUQP, HKUST");
	cout<<"QPUQP, HKUST"<<endl;

	DXUTCreateDevice( true, 800, 600);
    DXUTMainLoop(); // Enter into the DXUT render loop

	return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SettingUI.Init( &g_DialogResourceManager );

	g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 25, iY, 150, 25);
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 25, iY += 27, 150, 25, VK_F2 );

	g_SettingUI.SetCallback( OnGUIEvent ); iY = 10;

	g_SettingUI.AddStatic( IDC_LUMINANCELABEL, L"luminance: 0.10", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_LUMINANCE, 20, iY += 20, 125, 22, 1, 1000, 100);
	
	//yk: init pos, not meaningful
	//load table
	g_SettingUI.AddButton( IDC_LoadTable, L"load table", 0, iY-20, 80, 22 );
	g_SettingUI.AddStatic(IDC_MeaLabel, L"measure:", 0, iY-20, 70, 23 );
	g_SettingUI.AddComboBox( IDC_Mea, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_Mea )->SetDropHeight( 80 );
	g_SettingUI.AddStatic(IDC_AggrLabel, L"aggr.:", 0, iY-20, 60, 23 );
	g_SettingUI.AddComboBox( IDC_Aggr, 0, iY += 24, 80, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_Aggr )->SetDropHeight( 80 );
	//display attribute
	g_SettingUI.AddStatic(IDC_XShowLabel, L"x axis:", 0, iY, 50, 23 );
	g_SettingUI.AddComboBox( IDC_XShow, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_XShow )->SetDropHeight( 40 );
	g_SettingUI.AddComboBox( IDC_XLevel, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_XLevel )->SetDropHeight( 40 );
	g_SettingUI.AddStatic(IDC_YShowLabel, L"y axis:", 60, iY,  50, 23 );
	g_SettingUI.AddComboBox( IDC_YShow, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_YShow )->SetDropHeight( 40 );
	g_SettingUI.AddComboBox( IDC_YLevel, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_YLevel )->SetDropHeight( 40 );
	g_SettingUI.AddStatic(IDC_ZShowLabel, L"z axis:", 120, iY, 50, 23 );
	g_SettingUI.AddComboBox( IDC_ZShow, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_ZShow )->SetDropHeight( 40 );
	g_SettingUI.AddComboBox( IDC_ZLevel, 0, iY += 24, 100, 25, 0, false );
	g_SettingUI.GetComboBox( IDC_ZLevel )->SetDropHeight( 40 );
	//dice
	g_SettingUI.AddStatic(IDC_DiceLabel, L"Dice:", 0, iY, 50, 23 );
	g_SettingUI.AddCheckBox(IDC_DiceX, L"x", 0, iY, 35, 23);
	g_SettingUI.AddCheckBox(IDC_DiceY, L"y", 0, iY, 35, 23);
	g_SettingUI.AddCheckBox(IDC_DiceZ, L"z", 0, iY, 35, 23);
	g_SettingUI.AddButton( IDC_DiceSel, L"Select", 0, iY, 60, 22 );
	g_SettingUI.AddButton( IDC_DiceOrig, L"Original", 0, iY, 60, 22 );
	//color
	g_SettingUI.AddButton( IDC_BkgdCol, L"background color", 0, iY-40, 125, 22 );
	g_SettingUI.AddButton( IDC_PointCol, L"point color", 0, iY-40, 125, 22 );
	g_SettingUI.AddButton( IDC_GridCol, L"grid color", 0, iY-40, 125, 22 );
	g_SettingUI.AddButton( IDC_LabelCol, L"label color", 0, iY-40, 125, 22 );
	
	//
	g_SettingUI.AddStatic( IDC_ConDistLabel, L"context dist.: 10.0", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_ConDist, 20, iY += 20, 125, 22, 1, 20, 10 );
	
	//g_SettingUI.AddStatic( IDC_BrushRadLabel, L"brush radius: 1", 10, iY += 30, 125, 22 );
	//g_SettingUI.AddSlider( IDC_BrushRad, 20, iY += 20, 125, 22, 1, 100, 1);
	g_SettingUI.AddStatic( IDC_CoordScaleXLabel, L"x coord scale: 1.0", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_CoordScaleX, 20, iY += 20, 125, 22, 1, 100, int(g_coordScale.x * 10));
	g_SettingUI.AddStatic( IDC_CoordScaleYLabel, L"y coord scale: 1.0", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_CoordScaleY, 20, iY += 20, 125, 22, 1, 100, int(g_coordScale.y * 10));
	g_SettingUI.AddStatic( IDC_CoordScaleZLabel, L"z coord scale: 1.0", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_CoordScaleZ, 20, iY += 20, 125, 22, 1, 100, int(g_coordScale.z * 10));
	g_SettingUI.AddStatic( IDC_SampleConLabel, L"context sample: 1", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_SampleCon, 20, iY += 20, 125, 22, 1, 20, g_sampleCon);
	g_SettingUI.AddStatic( IDC_SampleFocLabel, L"focus sample: 1", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_SampleFoc, 20, iY += 20, 125, 22, 1, 20, g_sampleFoc);
	g_SettingUI.AddStatic( IDC_SpriteSizeLabel, L"particle size: 15", 10, iY += 30, 125, 22 );
	g_SettingUI.AddSlider( IDC_SpriteSize, 20, iY += 20, 125, 22, 1, 100, g_spriteSize * 1000);

	g_Camera.SetRotateButtons( true, false, false );
    g_MCamera.SetButtonMasks( MOUSE_RIGHT_BUTTON, 0, 0 );
    g_LCamera.SetButtonMasks( MOUSE_MIDDLE_BUTTON, 0, 0 );

    // Initialize the scaling and translation for the background meshes
    // Hardcode the matrices since we only have two.
    D3DXMATRIX m;
    D3DXMatrixTranslation( &g_mWorldBack[0], 0.0f, 2.0f, 0.0f );
    D3DXMatrixScaling( &g_mWorldBack[1], 0.3f, 0.3f, 0.3f );
    D3DXMatrixTranslation( &m, 0.0f, 1.5f, 0.0f );
    D3DXMatrixMultiply( &g_mWorldBack[1], &g_mWorldBack[1], &m );

    D3DXMatrixIdentity( &g_mWorldScaling );
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
   pDeviceSettings->d3d10.AutoDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if((DXUT_D3D10_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE) )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }
    return true;
}

void DrawText(ID3D10Device* pd3dDevice)
{
    UINT nBackBufferHeight = DXUTGetDXGIBackBufferSurfaceDesc()->Height;
    UINT nBackBufferWidth = DXUTGetDXGIBackBufferSurfaceDesc()->Width;

    //Output statistics
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    //g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );  
	g_pTxtHelper->DrawTextLine( DXUTGetFrameStats(true) );
	g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
	
	//dice
	for(int i = 0; i < 3; ++i)
	{
		WCHAR wstrHitStat[256];
		if (g_schema->bDice[i] && (g_schema->showAt[i] >= 0))
		{
			if(g_schema->diceS[i] != g_schema->diceE[i])
				StringCchPrintf(wstrHitStat, 256, L"Dicing %s: %d - %d",
					g_schema->levelNames[g_schema->showAt[i]][g_schema->curLevels[g_schema->showAt[i]]],
					g_schema->diceS[i], g_schema->diceE[i]);
			else
				StringCchPrintf(wstrHitStat, 256, L"Slicing %s = %d",
					g_schema->levelNames[g_schema->showAt[i]][g_schema->curLevels[g_schema->showAt[i]]],
					g_schema->diceS[i], g_schema->diceE[i]);
			g_pTxtHelper->DrawTextLine( wstrHitStat ); 
		}
	}
	//picked
	if(g_bPickSuccess)
	{
		WCHAR wstrHitStat[256];
		g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
		StringCchPrintf(wstrHitStat, 256, L"%s: %.1f", c_AggrNms[g_schema->aggrType], g_picked.pix.x); //todo: easy to extend to multiple measures
		g_pTxtHelper->DrawTextLine( wstrHitStat ); //top line in Text Region
		g_pTxtHelper->SetInsertionPos(g_picked.pos.x, g_picked.pos.y);
		StringCchPrintf(wstrHitStat, 256, L"     %s: %.1f", c_AggrNms[g_schema->aggrType], g_picked.pix.x); //todo: easy to extend to multiple measures
		g_pTxtHelper->DrawTextLine( wstrHitStat ); //side line beside mouse
	}
	// Draw help
    if(g_bShowHelp)
    {
        g_pTxtHelper->SetInsertionPos( 10, nBackBufferHeight - 200 );
        g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        g_pTxtHelper->DrawTextLine( L"Controls (F1 to hide):" );
        g_pTxtHelper->SetInsertionPos( 20, nBackBufferHeight - 180);
		g_pTxtHelper->DrawTextLine( 
			L"Left mouse: rotate view direction\n"
			L"Right mouse: rotate mesh\n"
			L"Middle mouse: pick\n"
			L"W/S/A/D/Q/E key: move\n"
			L"F4 key: Settings panel on/off\n"
			L"F5 key: Context region on/off\n"
			L"Quit: ESC" );
    }
    else
    {
        g_pTxtHelper->SetInsertionPos( 10, nBackBufferHeight-25 );
        g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        g_pTxtHelper->DrawTextLine( L"Press F1 for help" );
    }
	g_pTxtHelper->End();
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    g_Camera.FrameMove( fElapsedTime );
    g_MCamera.FrameMove( fElapsedTime );
    g_LCamera.FrameMove( fElapsedTime );
}

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    // Pass messages to settings dialog if its active
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }
    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SettingUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );
    g_MCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
    g_LCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
    return 0;
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	static D3DXVECTOR3 g_oldAt;
	D3DXVECTOR3 eye;
	D3DXVECTOR3 at1;

	D3DXVECTOR3 a = D3DXVECTOR3(1.6667, 0.690357, 1.28285);
	D3DXVECTOR3 b = D3DXVECTOR3(0.943945, 0.384375, 0.663165);
	D3DXVECTOR3 c = D3DXVECTOR3(1.6, 0.5, 1.4);
	D3DXVECTOR3 d = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 e = D3DXVECTOR3(1.89608, 0.585244, 0.999158);
	D3DXVECTOR3 f = D3DXVECTOR3(0.296078, 0.0852442, -0.400842);
	int pid;	

    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: 
				g_bShowHelp = !g_bShowHelp; 
				break;
			case VK_F4: 
				g_bShowSetting = !g_bShowSetting; 
				break;
			case VK_F5: 
				g_bDrawContext = !g_bDrawContext; 
				break;
			case 'R': //reset
				g_MCamera.Reset();
				g_Camera.Reset();
				g_LCamera.Reset();

				g_Camera.SetViewParams(&a, &b);
				g_LCamera.SetViewParams(&c, &d);
				g_MCamera.SetViewParams(&e, &f);
				break;
			case 'G':
				g_bDrawGrid = !g_bDrawGrid;
				break;
			case 'V':
				g_bDrawEnv = !g_bDrawEnv;
				break;
			case '0':	//0 key: recover random view
				g_pPlot->diceActivePlaneId = -1;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
				pid = (nChar - '1');
				if(g_schema->bDice[pid / 2] && !g_schema->bDiceSel) //currently being diced but not selected
					g_pPlot->diceActivePlaneId = pid;
				break;
        }
    }
}

void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext )
{
	//deb to tune camera pos
	//D3DXVECTOR3 a = *g_Camera.GetEyePt();
	//D3DXVECTOR3 b = *g_Camera.GetLookAtPt();
	//D3DXVECTOR3 c = *g_LCamera.GetEyePt();
	//D3DXVECTOR3 d = *g_LCamera.GetLookAtPt();
	//D3DXVECTOR3 e = *g_MCamera.GetEyePt();
	//D3DXVECTOR3 f = *g_MCamera.GetLookAtPt();
    if(nMouseWheelDelta != 0)
	{
		g_schema->MoveDicePlane(g_pPlot->diceActivePlaneId, nMouseWheelDelta / WHEEL_DELTA);
		//g_pPlot->MoveDicePlane(nMouseWheelDelta / WHEEL_DELTA, g_schema);
		g_pPlot->UpdateDice(g_pPlot->diceActivePlaneId / 2, g_schema);
		return;
	}
    bool bOldLeftButtonDown = g_bLeftButtonDown;
    bool bOldRightButtonDown = g_bRightButtonDown;
    bool bOldMiddleButtonDown = g_bMiddleButtonDown;
    g_bLeftButtonDown = bLeftButtonDown;
    g_bMiddleButtonDown = bMiddleButtonDown;
    g_bRightButtonDown = bRightButtonDown;

    if( bOldLeftButtonDown && !g_bLeftButtonDown )
        g_Camera.SetEnablePositionMovement( false );
    else if( !bOldLeftButtonDown && g_bLeftButtonDown )
        g_Camera.SetEnablePositionMovement( true );

    if( bOldRightButtonDown && !g_bRightButtonDown )
    {
        g_MCamera.SetEnablePositionMovement( false );
    }
    else if( !bOldRightButtonDown && g_bRightButtonDown )
    {
        g_MCamera.SetEnablePositionMovement( true );
        g_Camera.SetEnablePositionMovement( false );
    }
    if( bOldMiddleButtonDown && !g_bMiddleButtonDown )
    {
        g_LCamera.SetEnablePositionMovement( false );
    } 
    else if( !bOldMiddleButtonDown && g_bMiddleButtonDown )
    {
        g_LCamera.SetEnablePositionMovement( true );
        g_Camera.SetEnablePositionMovement( false );
    }
    // If no mouse button is down at all, enable camera movement.
    if( !g_bLeftButtonDown && !g_bRightButtonDown && !g_bMiddleButtonDown )
        g_Camera.SetEnablePositionMovement( true );
}

void SetLevelUIOnUpdateShow(int IDC, int xyz) //should update "level" accordingly
{
	g_SettingUI.GetComboBox(IDC)->RemoveAllItems();
	int dim = g_schema->showAt[xyz];
	if(dim >= 0)
	{
		for(int i = 0; i < g_schema->nLevels[dim]; ++i)
			g_SettingUI.GetComboBox(IDC)->AddItem(g_schema->levelNames[dim][i], (LPVOID)i );
		g_SettingUI.GetComboBox(IDC)->SetSelectedByIndex(g_schema->curLevels[dim]);
	}
}

void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{   
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_CHANGEDEVICE:     g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;
        case IDC_LUMINANCE:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				g_lumi = float( ((CDXUTSlider *)pControl)->GetValue() ) / 1000.f;
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"luminance: %.3f", g_lumi);
				g_SettingUI.GetStatic( IDC_LUMINANCELABEL )->SetText(wszText);
			}
			break;
		case IDC_ConDist:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				g_conDist = float( ((CDXUTSlider *)pControl)->GetValue() );
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"context dist.: %.1f", g_conDist);
				g_SettingUI.GetStatic( IDC_ConDistLabel )->SetText(wszText);
			}
			break;
		case IDC_BrushRad:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				g_brushRad = ((CDXUTSlider *)pControl)->GetValue();
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"brush radius: %d", g_brushRad);
				g_SettingUI.GetStatic( IDC_BrushRadLabel )->SetText(wszText);
			}
			break;
        case IDC_BACKGROUND:
            if( nEvent == EVENT_COMBOBOX_SELECTION_CHANGED )
            {
                g_nCurrBackground = (int)(size_t)g_SettingUI.GetComboBox( IDC_BACKGROUND )->GetSelectedData();
            }
            break;
		case IDC_CoordScaleX:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				g_coordScale.x = float( ((CDXUTSlider *)pControl)->GetValue() ) / 10.f;
				g_pPlot->coordScale = g_coordScale;
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"x coord scale: %.1f", g_coordScale.x);
				g_SettingUI.GetStatic(IDC_CoordScaleXLabel)->SetText(wszText);
				g_changedAxis = 0;
			}
			break;
		case IDC_CoordScaleY:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				g_coordScale.y = float( ((CDXUTSlider *)pControl)->GetValue() ) / 10.f;
				g_pPlot->coordScale = g_coordScale;
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"y coord scale: %.1f", g_coordScale.y);
				g_SettingUI.GetStatic(IDC_CoordScaleYLabel)->SetText(wszText);
				g_changedAxis = 1;
			}
			break;
		case IDC_CoordScaleZ:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				g_coordScale.z = float( ((CDXUTSlider *)pControl)->GetValue() ) / 10.f;
				g_pPlot->coordScale = g_coordScale;
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"z coord scale: %.1f", g_coordScale.z);
				g_SettingUI.GetStatic(IDC_CoordScaleZLabel)->SetText(wszText);
				g_changedAxis = 2;
			}
			break;
		case IDC_SampleCon:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				int rate = ( ((CDXUTSlider *)pControl)->GetValue() );
				g_sampleCon = 1 << (rate - 1);
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"context sample: %d", rate);
				g_SettingUI.GetStatic( IDC_SampleConLabel )->SetText(wszText);
			}
			break;
		case IDC_SampleFoc:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				int rate = ( ((CDXUTSlider *)pControl)->GetValue() );
				g_sampleFoc = 1 << (rate - 1);
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"focus sample: %d", rate);
				g_SettingUI.GetStatic( IDC_SampleFocLabel )->SetText(wszText);
			}
			break;
		case IDC_SpriteSize:
			if( nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				int tmp = ((CDXUTSlider *)pControl)->GetValue();
				g_spriteSize = float( tmp ) / 1000.f;
				WCHAR wszText[50];
				StringCchPrintf(wszText, 50, L"particle size: %d", tmp);
				g_SettingUI.GetStatic( IDC_SpriteSizeLabel )->SetText(wszText);
			}
			break;
		case IDC_XShow:
			g_schema->showAt[0] = g_SettingUI.GetComboBox( IDC_XShow )->GetSelectedIndex();
			if(g_schema->showAt[0] >= _nDim) //null
			{
				g_schema->showAt[0] = -1;
				g_SettingUI.GetCheckBox(IDC_DiceX)->SetChecked(false);
			}
			g_changedAxis = 0;
			SetLevelUIOnUpdateShow(IDC_XLevel, 0);
			break;
		case IDC_YShow:
			g_schema->showAt[1] = g_SettingUI.GetComboBox( IDC_YShow )->GetSelectedIndex();
			if(g_schema->showAt[1] >= _nDim) //null
			{
				g_schema->showAt[1] = -1;
				g_SettingUI.GetCheckBox(IDC_DiceY)->SetChecked(false);
			}
			g_changedAxis = 1;
			SetLevelUIOnUpdateShow(IDC_YLevel, 1);
			break;
		case IDC_ZShow:
			g_schema->showAt[2] = g_SettingUI.GetComboBox( IDC_ZShow )->GetSelectedIndex();
			if(g_schema->showAt[2] >= _nDim) //null
			{
				g_schema->showAt[2] = -1;
				g_SettingUI.GetCheckBox(IDC_DiceZ)->SetChecked(false);
			}
			g_changedAxis = 2;
			SetLevelUIOnUpdateShow(IDC_ZLevel, 2);
			break;
		case IDC_XLevel:
			if(g_schema->showAt[0] >= 0)	
				g_schema->curLevels[g_schema->showAt[0]] = g_SettingUI.GetComboBox(IDC_XLevel)->GetSelectedIndex();
			g_changedAxis = 0;
			break;
		case IDC_YLevel:
			if(g_schema->showAt[1] >= 0)	
				g_schema->curLevels[g_schema->showAt[1]] = g_SettingUI.GetComboBox(IDC_YLevel)->GetSelectedIndex();
			g_changedAxis = 1;
			break;
		case IDC_ZLevel:
			if(g_schema->showAt[2] >= 0)	
				g_schema->curLevels[g_schema->showAt[2]] = g_SettingUI.GetComboBox(IDC_ZLevel)->GetSelectedIndex();
			g_changedAxis = 2;
			break;
		case IDC_Mea:
			g_schema->meaAt = g_SettingUI.GetComboBox(IDC_Mea)->GetSelectedIndex();
			break;
		case IDC_Aggr:
			g_schema->aggrType = (AggrType)g_SettingUI.GetComboBox(IDC_Aggr)->GetSelectedIndex();
			break;
		case IDC_DiceX:
			if(g_schema->showAt[0] < 0) //not shown
				g_SettingUI.GetCheckBox(IDC_DiceX)->SetChecked(false);
			g_schema->bDice[0] = g_SettingUI.GetCheckBox(IDC_DiceX)->GetChecked();
			if(!g_schema->bDice[0])
				g_schema->ResetDiceSE(0);
			break;
		case IDC_DiceY:
			if(g_schema->showAt[1] < 0)
				g_SettingUI.GetCheckBox(IDC_DiceY)->SetChecked(false);
			g_schema->bDice[1] = g_SettingUI.GetCheckBox(IDC_DiceY)->GetChecked();
			if(!g_schema->bDice[1])
				g_schema->ResetDiceSE(1);
			break;
		case IDC_DiceZ:
			if(g_schema->showAt[2] < 0)
				g_SettingUI.GetCheckBox(IDC_DiceZ)->SetChecked(false);
			g_schema->bDice[2] = g_SettingUI.GetCheckBox(IDC_DiceZ)->GetChecked();
			if(!g_schema->bDice[2])
				g_schema->ResetDiceSE(2);
			break;
		case IDC_DiceSel:
			g_schema->bDiceSel = (g_schema->bDice[0] ||	g_schema->bDice[1] || g_schema->bDice[2]);
			g_SettingUI.GetCheckBox(IDC_DiceX)->SetChecked(false);
			g_SettingUI.GetCheckBox(IDC_DiceY)->SetChecked(false);
			g_SettingUI.GetCheckBox(IDC_DiceZ)->SetChecked(false);
			break;
		case IDC_DiceOrig:
			g_schema->bDice[0] = g_schema->bDice[1] = g_schema->bDice[2] = g_schema->bDiceSel = false;
			for(int i = 0; i < 3; ++i)
			{
				g_schema->ResetDiceSE(i);
				g_pPlot->UpdateDice(i, g_schema);
			}
			g_SettingUI.GetCheckBox(IDC_DiceX)->SetChecked(false);
			g_SettingUI.GetCheckBox(IDC_DiceY)->SetChecked(false);
			g_SettingUI.GetCheckBox(IDC_DiceZ)->SetChecked(false);
			break;
	}
}
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}

void InitTexBack(ID3D10Device* pd3dDevice)
{
	//////////////////////////////////////////////////////////////////////////init tmpTex
	SAFE_RELEASE(g_pSRVBack);
	SAFE_RELEASE(g_pRTVBack);
	SAFE_RELEASE(g_pTexBack);
	D3D10_VIEWPORT VP;
	UINT cVPs = 1;
	pd3dDevice->RSGetViewports(&cVPs, &VP);
	D3D10_TEXTURE2D_DESC descTex;
	DXNewTex(descTex, &g_pTexBack, (int)VP.Width, (int)VP.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_USAGE_DEFAULT, 
		D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE, 0, NULL, pd3dDevice);
	DXNewTexSRView(&g_pSRVBack, pd3dDevice, g_pTexBack, descTex);
	DXNewTexRTView(&g_pRTVBack, pd3dDevice, g_pTexBack, descTex);
}

void InitResource(ID3D10Device* pd3dDevice)
{		
	//////////////////////////////////////////////////////////////////////////data
	g_schema = new OlapSchema;
	if(g_bLoadFile)
	{
		g_schema->LoadData("concil.bin", 16);
	}
	else
	{
#if _bDebug
		WCHAR dimNm[_nDim][16] = {L"D1", L"D2", L"D3", L"D4"};
		WCHAR meaNm[_nMea][16] = {L"M1", L"M2", L"M3", L"M4"};
		WCHAR levelNm[_nDim][10][16] = {{L"D1.L1", L"D1.L2", L"D1.L3", L"D1.L4"},
			{L"D2.L1", L"D2.L2", L"D2.L3", L"D2.L4"},
			{L"D3.L1", L"D3.L2", L"D3.L3", L"D3.L4"},
		{L"D4.L1", L"D4.L2", L"D4.L3", L"D4.L4"}};
		int lv[_nDim] = {4, 4, 4, 4};
		int fo[_nDim][10] = {{2, 2, 2, 2}, {2, 2, 2, 2}, {2, 2, 2, 2}, {2, 2, 2, 2}}; //constraint: fo[0]*fo[1]*.. = sc
		int nRec = 16;
#else
		WCHAR dimNm[_nDim][16] = {L"Product", L"Customer", L"Channel", L"Time"};
		WCHAR meaNm[_nMea][16] = {L"Units", L"Dollar", L"Inventory", L"ProdCost"};
		WCHAR levelNm[_nDim][10][16] = {{L"Division", L"Line", L"Family", L"Group", L"Class", L"Code"},
		{L"Retailer", L"Store"}, {L"Base"}, {L"Year", L"Quarter", L"Month"}};
		int lv[_nDim] = {6, 2, 1, 3};
		int fo[_nDim][10] = {{2, 4, 3, 4, 10, 10}, {10, 10}, {9}, {2, 4, 2}}; //constraint: fo[0]*fo[1]*.. = sc
		int nRec = _M(10);
#endif
		memcpy(g_schema->nLevels, lv, sizeof(int) * _nDim);
		for(int i = 0; i < _nDim; ++i)
		{
			wcscpy_s(g_schema->dimNames[i], 16, dimNm[i]);
			for(int j = 0; j < lv[i]; ++j)
				wcscpy_s(g_schema->levelNames[i][j], 16, levelNm[i][j]);
			memcpy(g_schema->fanouts[i], fo[i], sizeof(int) * 10);
			g_schema->nScales[i][0] = fo[i][0];
			for(int j = 1; j < lv[i]; ++j)
				g_schema->nScales[i][j] = g_schema->nScales[i][j - 1] * fo[i][j];
			memcpy(g_schema->fanouts[i], fo[i], sizeof(int) * 10);
			g_schema->curLevels[i] = 0; //root //lv[i] - 1; //all start from leaf
		}
		for(int i = 0; i < _nMea; ++i)
			wcscpy_s(g_schema->meaNames[i], 16, meaNm[i]);

		//fact table
		g_schema->nRec = nRec;
		g_schema->pDim = new DimAttr[nRec];
		g_schema->pMea = new MeaAttr[nRec];
		for (int i = 0; i < nRec; ++i)
		{
			for (int j = 0; j < _nDim; ++j)
				g_schema->pDim[i][j] = _bDebug ? i : rand() % g_schema->nScales[j][lv[j] - 1];
			g_schema->pMea[i][0] = _bDebug ? i : rand() % _MaxMea0;
			g_schema->pMea[i][1] = _bDebug ? i : rand() % _MaxMea1;
			g_schema->pMea[i][2] = _bDebug ? i : rand() % _MaxMea0;
			g_schema->pMea[i][3] = _bDebug ? i : rand() % _MaxMea0;
			
			//ykdeb demo: //channel??
			//if (Time.M in[0,5)&& Cus.Ret is 20||60 && Prod.Fa in 10-20) then re-rand mea0 in [0,M/4];
			//if (Time.M in[10,16)&& Cus.Ret is 30||50||80 && Prod.Fa in 10-20) then re-rand mea0 in [0,4M];
			int M = g_schema->pDim[i][3];
			int Ret = g_schema->pDim[i][1] / 10;
			int Fa = g_schema->pDim[i][0] / 400;
			if(M < 5 && (Ret == 2 || Ret == 4 || Ret == 9) && (Fa >= 10 && Fa < 20))
				g_schema->pMea[i][0] = rand() % (5);
			else if(M >= 10 && (Ret == 3 || Ret == 5 || Ret == 8) && (Fa >= 10 && Fa < 20))
				g_schema->pMea[i][0] = rand() % (_MaxMea0 * 50);
			else
				g_schema->pMea[i][0] = rand() % (_MaxMea0 * 10);
			
		}
		//other settings
		g_schema->aggrType = AggrSum;
	}
	
	//////////////////////////////////////////////////////////////////////////resource
	//new Dx for R, S, build index on S.indexedAttr
	g_pTable = new DXOlapTable(g_schema, pd3dDevice);
	SAFE_DELETE_ARRAY(g_schema->pDim);
	SAFE_DELETE_ARRAY(g_schema->pMea);

	/////////////////////////////////////////////////////////////////////////scatterplot
	WCHAR str[MAX_PATH];
	g_pTechDrawPlot = g_pEffect->GetTechniqueByName( "techScatterPlot" );
	g_pPlot = new DXScatterPlot;
	//axes
	D3D10_PASS_DESC PassDesc;
	g_pTechDrawPlot->GetPassByName("axesCon")->GetDesc(&PassDesc);
	UINT numElements = (UINT)ceilf((float)sizeof(DESC_POS3COL3) / (float)sizeof(DESC_POS3COL3[0]));
	V( pd3dDevice->CreateInputLayout(DESC_POS3COL3, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLayoutPos3Col3));
	int nAxesVerts = 3 * 2; //3 axis lines
	g_pPlot->coordScale = g_coordScale;
	g_pPlot->SetPlotOrig(g_coordScale);
	g_pPlot->pAxesVert = new Pos3fCol3f[nAxesVerts];  
	g_pPlot->DXInitAxisVerts(g_coordScale);
	DXNewVB<Pos3fCol3f>(&g_pPlot->pAxesVB, pd3dDevice, g_pPlot->pAxesVert, nAxesVerts);
	for(int i = 0; i < 3; ++i)
	{
		g_schema->OnShowAtUpdated(i);
		g_pPlot->UpdateGrid(i, g_schema, pd3dDevice);
		g_pPlot->UpdateDice(i, g_schema);
	}

	//points: no resource to prepare

	//////////////////////////////////////////////////////////////////////////context
	//context boarder
	g_pTechContextBorder = g_pEffect->GetTechniqueByName("techContextBorder");
	g_pTechContextBorder->GetPassByIndex(0)->GetDesc(&PassDesc);
	numElements = (UINT)ceilf((float)sizeof(DESC_POS3) / (float)sizeof(DESC_POS3[0]));
	V(pd3dDevice->CreateInputLayout(DESC_POS3, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLayoutPos3));
	Pos3f pBoarderLineVerts[] =
	{
		{D3DXVECTOR3(-0.99f, 0.99f,	0.5f)},		{D3DXVECTOR3(0.99f, 0.99f,	0.5f)},	
		{D3DXVECTOR3(-0.99f, 0.99f,	0.5f)},		{D3DXVECTOR3(-0.99f, -0.99f,0.5f)},	
		{D3DXVECTOR3(0.99f, 0.99f,	0.5f)},		{D3DXVECTOR3(0.99f, -0.99f,	0.5f)},	
		{D3DXVECTOR3(-0.99f, -0.99f,0.5f)},		{D3DXVECTOR3(0.99f, -0.99f,	0.5f)},	
	};
	DXNewVB<Pos3f>(&g_pContextBorderVB, pd3dDevice, pBoarderLineVerts, c_nVContextBorder);

	//little focus
	g_pTechLittleFocus = g_pEffect->GetTechniqueByName("techLittleFocus");
	//use contextborder's layout
	float zn = g_Camera.GetNearClip();
	Pos3f pLittleFocusVerts[] =
	{
		{D3DXVECTOR3(-1, 1,	zn)},		{D3DXVECTOR3(1, 1,	zn)},	
		{D3DXVECTOR3(-1, 1,	zn)},		{D3DXVECTOR3(-1, -1,zn)},	
		{D3DXVECTOR3(1, 1,	zn)},		{D3DXVECTOR3(1, -1,	zn)},	
		{D3DXVECTOR3(-1, -1,zn)},		{D3DXVECTOR3(1, -1,	zn)},	
	};
	DXNewVB<Pos3f>(&g_pLittleFocusVB, pd3dDevice, pLittleFocusVerts, c_nVContextBorder);

	//////////////////////////////////////////////////////////////////////////tex
	V(DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Particle.dds"));
	V(D3DX10CreateShaderResourceViewFromFile( pd3dDevice, str, NULL, NULL, &g_pParticleSRV, NULL));

	//////////////////////////////////////////////////////////////////////////GUI
	for(int i = 0; i < _nDim; ++i)
	{
		g_SettingUI.GetComboBox( IDC_XShow )->AddItem(g_schema->dimNames[i], (LPVOID)i );
		g_SettingUI.GetComboBox( IDC_YShow )->AddItem(g_schema->dimNames[i], (LPVOID)i );
		g_SettingUI.GetComboBox( IDC_ZShow )->AddItem(g_schema->dimNames[i], (LPVOID)i );
	}
	for(int i = 0; i < _nMea; ++i)
		g_SettingUI.GetComboBox( IDC_Mea )->AddItem(g_schema->meaNames[i], (LPVOID)i );
	for(int i = 0; i < sizeof(c_AggrNms)/sizeof(c_AggrNms[0]); ++i)
		g_SettingUI.GetComboBox( IDC_Aggr )->AddItem(c_AggrNms[i], (LPVOID)i );
	g_SettingUI.GetComboBox( IDC_Aggr )->SetSelectedByIndex(g_schema->aggrType);

	g_SettingUI.GetComboBox( IDC_XShow )->AddItem( L"(null)", (LPVOID)_nDim);
	g_SettingUI.GetComboBox( IDC_YShow )->AddItem( L"(null)", (LPVOID)_nDim);
	g_SettingUI.GetComboBox( IDC_ZShow )->AddItem( L"(null)", (LPVOID)_nDim);
	g_SettingUI.GetComboBox( IDC_XShow )->SetSelectedByIndex(g_schema->showAt[0]);
	g_SettingUI.GetComboBox( IDC_YShow )->SetSelectedByIndex(g_schema->showAt[1]);
	g_SettingUI.GetComboBox( IDC_ZShow )->SetSelectedByIndex(g_schema->showAt[2]);
	
	SetLevelUIOnUpdateShow(IDC_XLevel, 0);
	SetLevelUIOnUpdateShow(IDC_YLevel, 1);
	SetLevelUIOnUpdateShow(IDC_ZLevel, 2);

	//////////////////////////////////////////////////////////////////////////env
	//Env vb&layout
	DXNewCubeVB(&g_pEnvVB, &g_pEnvVBInd, pd3dDevice, g_coordScale);
	g_pTechEnv = g_pEffect->GetTechniqueByName( "techEnv" );

	//////////////////////////////////////////////////////////////////////////backup state
	//enableDS
	D3D10_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE; 
	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D10_COMPARISON_LESS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0;
	dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_GREATER;
	dsDesc.FrontFace.StencilDepthFailOp =
		dsDesc.FrontFace.StencilPassOp =
		dsDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	dsDesc.BackFace = dsDesc.FrontFace;
	V(pd3dDevice->CreateDepthStencilState(&dsDesc, &g_pDSEnableDS));

	//////////////////////////////////////////////////////////////////////////other resources
	DXNewQuadVBPos3(&g_pVBQuadPos3, pd3dDevice);
	DXNewQuadVBPos3Tex2(&g_pVBQuadPos3Tex2, pd3dDevice);
	g_pPlot->NewDicePlaneVBs(pd3dDevice);
	
	g_pTechDrawPlot = g_pEffect->GetTechniqueByName("techScatterPlot");
	g_pTechDrawPlot->GetPassByName("spritesAvg")->GetDesc(&PassDesc);
	numElements = (UINT)ceilf((float)sizeof(DESC_POS3TEX2) / (float)sizeof(DESC_POS3TEX2[0]));
	V(pd3dDevice->CreateInputLayout(DESC_POS3TEX2, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLayoutPos3Tex2));

	D3D10_TEXTURE2D_DESC descTex;
	DXNewTex(descTex, &g_pTexStage1Pix, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D10_USAGE_STAGING, 0, D3D10_CPU_ACCESS_READ, NULL, pd3dDevice);
}


void InitContextStencil(ID3D10Device* pd3dDevice)
{
	//////////////////////////////////////////////////////////////////////////context quad
	g_pTechContextQuad = g_pEffect->GetTechniqueByName("techContextQuad");

	//ds state
	D3D10_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = FALSE; 
	dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D10_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;
	dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilDepthFailOp =
		dsDesc.FrontFace.StencilPassOp =
		dsDesc.FrontFace.StencilFailOp =
		dsDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	dsDesc.BackFace = dsDesc.FrontFace;
	ID3D10DepthStencilState* pDSState;
	V(pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSState));
	pd3dDevice->OMSetDepthStencilState(pDSState, 0);

	//draw
	UINT stride = sizeof(Pos3f);
	UINT offset = 0;
	pd3dDevice->IASetVertexBuffers(0, 1, &g_pVBQuadPos3, &stride, &offset);
	pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDevice->IASetInputLayout(g_pLayoutPos3);
	float ClearColor[4] = { 0.0, 0.0, 0.0, 0.0 };
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	pd3dDevice->ClearRenderTargetView( pRTV, ClearColor);
	ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
	pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	D3D10_VIEWPORT VP;
	UINT cVPs = 1;
	pd3dDevice->RSGetViewports(&cVPs, &VP);
	const int conW = (int)VP.Width * cConSizeRatio;
	const int conH = (int)VP.Height * cConSizeRatio;
	D3D10_VIEWPORT vp;
	vp.Width = conW;
	vp.Height = conH;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = VP.Height - conH;
	pd3dDevice->RSSetViewports(1, &vp);
	DXDraw(pd3dDevice, g_pTechContextQuad, 6);
	//recover
	pd3dDevice->RSSetViewports(1, &VP);

	SAFE_RELEASE(pDSState);
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
    V_RETURN( g_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                                OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                                L"Arial", &g_pFont10 ) );
    V_RETURN( D3DX10CreateSprite( pd3dDevice, 512, &g_pSprite10 ) );
    g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont10, g_pSprite10, 15 );

	// Read the D3DX effect file
	WCHAR str[MAX_PATH];
	V(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"I3DC_draw.fx"));
	DXNewEffect(&g_pEffect, pd3dDevice, str);
    g_pmWorldViewProj = g_pEffect->GetVariableByName( "g_mWorldViewProj" )->AsMatrix();
    g_pDiffuseTex = g_pEffect->GetVariableByName( "g_txDiffuse" )->AsShaderResource();

	InitResource(pd3dDevice);

	//Setup the camera's view parameters
	D3DXVECTOR3 EyeInit(0.f, 1.0f, -1.7f);
	D3DXVECTOR3 AtInit (0.0f, 0.0f, -0.0f);
	g_Camera.SetViewParams(&EyeInit, &AtInit);
    g_LCamera.SetViewParams(&EyeInit, &AtInit);
    g_MCamera.SetViewParams(&EyeInit, &AtInit);

	ID3D10ShaderResourceView* pCubeRV = NULL;
	V_RETURN(DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Light Probes\\uffizi_cross.dds"));
	V(D3DX10CreateShaderResourceViewFromFile( pd3dDevice, str, NULL, NULL, &pCubeRV, NULL));
	ID3D10Texture2D* pCubeTexture = NULL;
	pCubeRV->GetResource( (ID3D10Resource**)&pCubeTexture );
	V_RETURN( g_envmap.OnD3D10CreateDevice( pd3dDevice, 50, pCubeTexture, pCubeRV, L"skybox10.fx" ) );
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10SwapChainResized( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr = S_OK;
    V_RETURN( g_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 500.0f );
	g_MCamera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_LCamera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SettingUI.SetLocation( 0, pBackBufferSurfaceDesc->Height- 410);
	g_SettingUI.SetSize( pBackBufferSurfaceDesc->Width, 300 );
	
    int iY = 0;
/*  g_SettingUI.GetControl( IDC_BACKGROUND )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
    g_SettingUI.GetControl( IDC_ENABLELIGHT )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
    g_SettingUI.GetControl( IDC_RENDERTYPE )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
*/
	g_SettingUI.GetControl( IDC_BkgdCol )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 0); //24 );
	g_SettingUI.GetControl( IDC_PointCol )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 24 );
	g_SettingUI.GetControl( IDC_GridCol )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 24 );
	g_SettingUI.GetControl( IDC_LabelCol )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 24 );

	g_SettingUI.GetControl( IDC_LUMINANCELABEL )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_LUMINANCE )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	//yk
	g_SettingUI.GetControl( IDC_SampleConLabel )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_SampleCon)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_SampleFocLabel)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_SampleFoc)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );

	g_SettingUI.GetControl( IDC_ConDistLabel )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_ConDist)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	//g_SettingUI.GetControl( IDC_BrushRadLabel )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	//g_SettingUI.GetControl( IDC_BrushRad)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_CoordScaleXLabel )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_CoordScaleX)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_CoordScaleYLabel )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_CoordScaleY)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_CoordScaleZLabel )->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_CoordScaleZ)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );

	g_SettingUI.GetControl( IDC_SpriteSizeLabel)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	g_SettingUI.GetControl( IDC_SpriteSize)->SetLocation( pBackBufferSurfaceDesc->Width - 145, iY += 20 );
	

	//data region
	g_SettingUI.GetControl( IDC_LoadTable)->SetLocation(pBackBufferSurfaceDesc->Width - 250, iY = 240);
	iY += 25;
	//"measure:"
	g_SettingUI.GetControl( IDC_MeaLabel)->SetLocation(pBackBufferSurfaceDesc->Width - 470, iY);
	g_SettingUI.GetControl( IDC_Mea)->SetLocation(pBackBufferSurfaceDesc->Width - 400, iY);
	//"aggr.:"
	g_SettingUI.GetControl( IDC_AggrLabel)->SetLocation(pBackBufferSurfaceDesc->Width - 305, iY);
	g_SettingUI.GetControl( IDC_Aggr)->SetLocation(pBackBufferSurfaceDesc->Width - 250, iY);
	iY += 5;
	//"x:"
	g_SettingUI.GetControl( IDC_XShowLabel)->SetLocation(pBackBufferSurfaceDesc->Width - 470, iY += 25);
	g_SettingUI.GetControl( IDC_XShow)->SetLocation(pBackBufferSurfaceDesc->Width - 420, iY);
	g_SettingUI.GetControl( IDC_XLevel)->SetLocation(pBackBufferSurfaceDesc->Width - 320, iY);
	//"y:"
	g_SettingUI.GetControl( IDC_YShowLabel)->SetLocation(pBackBufferSurfaceDesc->Width - 470, iY += 25);
	g_SettingUI.GetControl( IDC_YShow)->SetLocation(pBackBufferSurfaceDesc->Width - 420, iY);
	g_SettingUI.GetControl( IDC_YLevel)->SetLocation(pBackBufferSurfaceDesc->Width - 320, iY);
	//"z:"
	g_SettingUI.GetControl( IDC_ZShowLabel)->SetLocation(pBackBufferSurfaceDesc->Width - 470, iY += 25);
	g_SettingUI.GetControl( IDC_ZShow)->SetLocation(pBackBufferSurfaceDesc->Width - 420, iY);
	g_SettingUI.GetControl( IDC_ZLevel)->SetLocation(pBackBufferSurfaceDesc->Width - 320, iY);
	//dice
	g_SettingUI.GetControl( IDC_DiceLabel)->SetLocation(pBackBufferSurfaceDesc->Width - 470, iY += 25);
	g_SettingUI.GetControl( IDC_DiceX)->SetLocation(pBackBufferSurfaceDesc->Width - 420, iY);
	g_SettingUI.GetControl( IDC_DiceY)->SetLocation(pBackBufferSurfaceDesc->Width - 380, iY);
	g_SettingUI.GetControl( IDC_DiceZ)->SetLocation(pBackBufferSurfaceDesc->Width - 340, iY);
	g_SettingUI.GetControl( IDC_DiceSel)->SetLocation(pBackBufferSurfaceDesc->Width - 300, iY);
	g_SettingUI.GetControl( IDC_DiceOrig)->SetLocation(pBackBufferSurfaceDesc->Width - 230, iY);

	//////////////////////////////////////////////////////////////////////////other related resources
	if(g_bStartedRender)
	{
		InitContextStencil(pd3dDevice);
		InitTexBack(pd3dDevice);
	}
	g_envmap.OnD3D10ResizedSwapChain( pBackBufferSurfaceDesc );
    return S_OK;
}

void DrawAxes( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTech, ID3D10Buffer* pAxesVB, D3DXVECTOR3& orig)
{
	pd3dDevice->IASetInputLayout(g_pLayoutPos3Col3);
	UINT stride = sizeof(Pos3fCol3f);
	UINT offset = 0;
	pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	pd3dDevice->IASetVertexBuffers(0, 1, &pAxesVB, &stride, &offset);
	V((g_pEffect->GetVariableByName("v_orig")->AsVector())->SetFloatVector(orig));
	pTech->GetPassByIndex(0)->Apply(0);
	pd3dDevice->Draw(6, 0);
}

void DrawEnv(ID3D10Device* pd3dDevice, bool bFocus)
{
	pd3dDevice->IASetInputLayout(g_pLayoutPos3);
	UINT stride = sizeof(Pos3f);
	UINT offset = 0;
	pd3dDevice->IASetVertexBuffers(0, 1, &g_pEnvVB, &stride, &offset);
	pd3dDevice->IASetIndexBuffer(g_pEnvVBInd, DXGI_FORMAT_R32_UINT, 0);
	pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	D3D10_TECHNIQUE_DESC techDesc;
	g_pTechEnv->GetDesc(&techDesc);
	if(!bFocus)
	{
		g_pTechEnv->GetPassByName("context")->Apply(0);
		pd3dDevice->DrawIndexed(36, 0, 0);
	}
	else
	{
		g_pTechEnv->GetPassByName("focus")->Apply(0); 
		pd3dDevice->DrawIndexed(36, 0, 0);
	}
}

void DrawPlots(ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTech, DXOlapTable* pTable, DXScatterPlot* pPlot, 
			   OlapSchema* pSchema, bool bDrawFocus)
{
	//1. consts
	V((g_pEffect->GetVariableByName("v_orig")->AsVector())->SetFloatVector(pPlot->orig));
	D3DXVECTOR3 v_coordScale;
	for(int i = 0; i < 3; ++i)
		v_coordScale[i] = g_coordScale[i] / (float)g_schema->nScaleCur[i];
	V((g_pEffect->GetVariableByName("v_coordScale")->AsVector())->SetFloatVector(v_coordScale));
	V((g_pEffect->GetVariableByName("v_scaleRatio")->AsVector())->SetIntVector(g_schema->scaleRatioCur));
	//index of x,y,z in 1st texel; of measure in 2nd texel
	int xyzmAt[4] = {g_schema->showAt[0], g_schema->showAt[1], g_schema->showAt[2], g_schema->meaAt};
	V((g_pEffect->GetVariableByName("v_xyzmAt")->AsVector())->SetIntVector(xyzmAt));

	//2. records
	//gs sprite position for four triangle verts
	D3DXMATRIX matSpritePos( -g_spriteSize, g_spriteSize, 0, 0,		
		g_spriteSize, g_spriteSize, 0, 0,		
		-g_spriteSize, -g_spriteSize, 0, 0,		
		g_spriteSize, -g_spriteSize, 0, 0);
	D3DXMATRIX SPP = matSpritePos * *g_Camera.GetProjMatrix();
	pd3dDevice->IASetInputLayout(NULL);
	pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	V((g_pEffect->GetVariableByName("v_matSPP")->AsMatrix())->SetMatrix(SPP));
	V((g_pEffect->GetVariableByName("v_texParticle")->AsShaderResource())->SetResource(g_pParticleSRV));
	V((g_pEffect->GetVariableByName("v_lumi")->AsScalar())->SetFloat(g_lumi));
	assert((g_sampleCon & (g_sampleCon - 1)) == 0);
	assert((g_sampleFoc & (g_sampleFoc - 1)) == 0);
	V((g_pEffect->GetVariableByName("texTableDim")->AsShaderResource())->SetResource(pTable->pSRVDimData));
	V((g_pEffect->GetVariableByName("texTableMea")->AsShaderResource())->SetResource(pTable->pSRVMeaData));
	V((g_pEffect->GetVariableByName("texTableW")->AsScalar())->SetInt(pTable->texDataW));
	
	ID3D10RenderTargetView* pOldRTV;
	ID3D10DepthStencilView* pOldDSV;
	UINT stride = sizeof(Pos3fTex2f);
	UINT offset = 0;
	float ClearColor[4] = {0, 0, 0, 0};
	int zero3[3] = {0, 0, 0};
	int max3[3] = {INT_MAX, INT_MAX, INT_MAX};
	if(bDrawFocus) //blended sprites
	{
		V((g_pEffect->GetVariableByName("v_sampleRate")->AsScalar())->SetInt(g_sampleFoc));
		if(g_schema->bDiceSel) //don't draw the unselected
		{
			V((g_pEffect->GetVariableByName("diceS")->AsVector())->SetIntVector(g_schema->diceS));
			V((g_pEffect->GetVariableByName("diceE")->AsVector())->SetIntVector(g_schema->diceE));
		}
		else
		{
			V((g_pEffect->GetVariableByName("diceS")->AsVector())->SetIntVector(zero3));
			V((g_pEffect->GetVariableByName("diceE")->AsVector())->SetIntVector(max3));
		}

		switch (g_schema->aggrType)
		{
		case AggrSum:
			pTech->GetPassByName("spritesSum")->Apply(0);
			pd3dDevice->Draw(pTable->nRec / g_sampleFoc, 0);
			break;
		case AggrCnt:
			pTech->GetPassByName("spritesCnt")->Apply(0);
			pd3dDevice->Draw(pTable->nRec / g_sampleFoc, 0);
			break;
		case AggrAvg:
			//0. save old 
			pd3dDevice->OMGetRenderTargets( 1, &pOldRTV, &pOldDSV );
			//1. draw sum, cnt to texTmp
			pd3dDevice->OMSetRenderTargets(1, &g_pRTVBack, pOldDSV);
			pd3dDevice->ClearRenderTargetView(g_pRTVBack, ClearColor);
			pTech->GetPassByName("spritesSum")->Apply(0);
			pd3dDevice->Draw(pTable->nRec / g_sampleFoc, 0); 
			//2. draw a quad, output sum/cnt to screen
			pd3dDevice->IASetVertexBuffers(0, 1, &g_pVBQuadPos3Tex2, &stride, &offset);
			pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pd3dDevice->IASetInputLayout(g_pLayoutPos3Tex2);
			pd3dDevice->OMSetRenderTargets( 1, &pOldRTV, pOldDSV );
			V((g_pEffect->GetVariableByName("texTmp")->AsShaderResource())->SetResource(g_pSRVBack));
			pTech->GetPassByName("spritesAvg")->Apply(0); //avg = sum/cnt
			pd3dDevice->Draw(6, 0);
			//3. cleanup
			SAFE_RELEASE( pOldRTV );
			SAFE_RELEASE( pOldDSV );
			break;
		case AggrMax:
			pTech->GetPassByName("spritesMax")->Apply(0);
			pd3dDevice->Draw(pTable->nRec / g_sampleFoc, 0);
			break;
		case AggrMin:
			pTech->GetPassByName("spritesMinFloodMax")->Apply(0); //pre-flood
			pd3dDevice->Draw(pTable->nRec / g_sampleFoc, 0);
			pTech->GetPassByName("spritesMin")->Apply(0);
			pd3dDevice->Draw(pTable->nRec / g_sampleFoc, 0);
			break;
		default:
			assert(0);
		}
	}
	else //points
	{
		V((g_pEffect->GetVariableByName("v_sampleRate")->AsScalar())->SetInt(g_sampleCon));
		pTech->GetPassByName("points")->Apply(0);
		pd3dDevice->Draw(pTable->nRec / g_sampleCon, 0);
	}

	//3. grids
	stride = sizeof(Pos3f);
	offset = 0;
	if(bDrawFocus && g_bDrawGrid) //not shown in context
	{
		pd3dDevice->IASetInputLayout(g_pLayoutPos3);
		pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		V((g_pEffect->GetVariableByName("v_gridScale")->AsVector())->SetFloatVector(g_coordScale));
		for(int i = 0; i < 3; ++i) //3 axes
		{
			if(g_schema->nScaleCur[i] <= 0)
				continue;
			pd3dDevice->IASetVertexBuffers(0, 1, &pPlot->pGridsVB[i], &stride, &offset);
			pTech->GetPassByName("grids")->Apply(0);
			if(g_schema->nScaleCur[(i + 1) % 3] > 0) //the intersecting axis on 1st plane
				pd3dDevice->Draw(g_schema->nScaleCur[i] * 2, 0); //each axis point generates 2 lines on 2 sides
			if(g_schema->nScaleCur[(i + 2) % 3] > 0) //the intersecting axis on 2nd plane
				pd3dDevice->Draw(g_schema->nScaleCur[i] * 2, g_schema->nScaleCur[i] * 2);
		}

		//grid fonts
		if(bDrawFocus)
		{
			g_pTxtHelper->Begin();
			g_pTxtHelper->SetForegroundColor(D3DXCOLOR(0.3f, 1.f, 0.3f, 1.0f));
			D3D10_VIEWPORT VP;
			UINT cVPs = 1;
			pd3dDevice->RSGetViewports(&cVPs, &VP);
			D3DXVECTOR3 fscreen;
			WCHAR wstr[256];
			D3DXVECTOR3 fposWorld;
			D3DXVECTOR3 v[2]; //two end points on 2 planes

			for(int axId = 0; axId < 3; ++axId)
			{
				if(g_schema->nScaleCur[axId] <= 0) //this dim not displayed
					continue;
				int textItv = max(1, g_schema->nScaleCur[axId] / c_nTextsOnAxis);
				if(textItv > 1000)
					textItv = 1000;
				else if(textItv > 100)
					textItv = 100;
				else if(textItv > 10)
					textItv = 10;
				int nS =(int)ceilf((float)g_schema->nScaleCur[axId] / textItv); 
				float itv = 1.f / nS;
				for(int j = 1; j < nS; ++j) //!=0: not overlap with axis
				{
					switch (axId)
					{
					case 0:
						v[0] = D3DXVECTOR3(j * itv, 1.05f, -0.01f) * g_coordScale;
						v[1] = D3DXVECTOR3(j * itv, -0.01f, 1.05f) * g_coordScale;
						break;
					case 1:
						v[0] = D3DXVECTOR3(-0.01f, j * itv, 1.05f) * g_coordScale;
						v[1] = D3DXVECTOR3(1.05f, j * itv, -0.01f) * g_coordScale;
						break;
					case 2:
						v[0] = D3DXVECTOR3(1.05f, -0.01f, j * itv) * g_coordScale;
						v[1] = D3DXVECTOR3(-0.01f, 1.05f, j * itv) * g_coordScale;
						break;
					default:
						assert(0);
					}
					for(int nV = 0; nV <= 1; ++nV) //2 sides
					{
						if(g_schema->showAt[(axId + nV + 1) % 3] < 0) //this axis not used
							continue;
						fposWorld = g_pPlot->orig + v[nV];
						D3DXVec3Project(&fscreen, &fposWorld, (D3DVIEWPORT9*)&VP, g_Camera.GetProjMatrix(), 
							g_Camera.GetViewMatrix(), g_MCamera.GetWorldMatrix());
						if(fscreen.z >= 0.f && fscreen.z <= 1.f)
						{
							g_pTxtHelper->SetInsertionPos((int)fscreen.x, (int)fscreen.y);
							StringCchPrintf(wstr, 256, L"%d", j * textItv);
							g_pTxtHelper->DrawTextLine(wstr);
						}
					}
				}
			}
			g_pTxtHelper->End();
		}
	}

	//4. axes
	pd3dDevice->IASetInputLayout(g_pLayoutPos3Col3);
	stride = sizeof(Pos3fCol3f);
	offset = 0;
	pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	pd3dDevice->IASetVertexBuffers(0, 1, &pPlot->pAxesVB, &stride, &offset);
	if(bDrawFocus)
		pTech->GetPassByName("axesFoc")->Apply(0);
	else //con
		pTech->GetPassByName("axesCon")->Apply(0);
	pd3dDevice->Draw(6, 0);
	//axis fonts
	if(bDrawFocus)
	{
		g_pTxtHelper->Begin();
		g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.f, 1.f, 1.0f, 1.0f));
		D3D10_VIEWPORT VP;
		UINT cVPs = 1;
		pd3dDevice->RSGetViewports(&cVPs, &VP);
		D3DXVECTOR3 fscreen;
		WCHAR wstr[256];
		//orig
		D3DXVECTOR3 fposWorld = pPlot->orig;
		D3DXVec3Project(&fscreen, &fposWorld, (D3DVIEWPORT9*)&VP, g_Camera.GetProjMatrix(), 
			g_Camera.GetViewMatrix(), g_MCamera.GetWorldMatrix());
		if(fscreen.z >= 0.f && fscreen.z <= 1.f)
		{
			g_pTxtHelper->SetInsertionPos((int)fscreen.x, (int)fscreen.y);
			g_pTxtHelper->DrawTextLine(L"Council");
		}
		for(int j = 0; j < 3; ++j) //note: not nDim!!!
		{
			//axis
			if(g_schema->showAt[j] < 0) //this axis not used
				continue;
			fposWorld = pPlot->orig + pPlot->pAxesVert[j * 2 + 1].Pos * g_coordScale;
			D3DXVec3Project(&fscreen, &fposWorld, (D3DVIEWPORT9*)&VP, g_Camera.GetProjMatrix(), 
				g_Camera.GetViewMatrix(), g_MCamera.GetWorldMatrix());
			if(fscreen.z >= 0.f && fscreen.z <= 1.f)
			{
				g_pTxtHelper->SetInsertionPos((int)fscreen.x, (int)fscreen.y);
				StringCchPrintf(wstr, 256, L"%s.%s", g_schema->dimNames[g_schema->showAt[j]],
					g_schema->levelNames[g_schema->showAt[j]][g_schema->curLevels[g_schema->showAt[j]]]
				);
				g_pTxtHelper->DrawTextLine(wstr);
			}
		}
		g_pTxtHelper->End();
	}

	//5. draw dice planes //for each dim, draw 2 planes at ds[axId], de[axId]
	if(bDrawFocus && !g_schema->bDiceSel) //only draw planes when not selected out
	{
		for(int axId = 0; axId < 3; ++axId)
		{
			if((g_schema->nScaleCur[axId] <= 0) || !g_schema->bDice[axId]) //not displayed or not diced
				continue;
			UINT stride = sizeof(Pos3fCol3f);
			UINT offset = 0;
			pd3dDevice->IASetVertexBuffers(0, 1, &g_pPlot->pDicePlaneVB[axId], &stride, &offset);
			pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pd3dDevice->IASetInputLayout(g_pLayoutPos3Col3);
			V((g_pEffect->GetVariableByName("v_dicePlaneOff")->AsVector())->SetFloatVector(g_pPlot->dicePlaneOff[axId * 2]));
			pTech->GetPassByName("dicePlane")->Apply(0);
			pd3dDevice->Draw(6, 0); //start plane
			V((g_pEffect->GetVariableByName("v_dicePlaneOff")->AsVector())->SetFloatVector(g_pPlot->dicePlaneOff[axId * 2 + 1]));
			pTech->GetPassByName("dicePlane")->Apply(0);
			pd3dDevice->Draw(6, 0); //end plane
		}
	}
}

//pick a viewport pixel's color (in full backbuffer format)
void Pick(ID3D10Device* pd3dDevice)
{
	//1. Draw the aggregated point as billboard (untextured quad sprite) to texTmp
	float ClearColor[4] = {0, 0, 0, 0}; 
	ID3D10RenderTargetView* pOldRTV;
	ID3D10DepthStencilView* pOldDSV;
	pd3dDevice->OMGetRenderTargets( 1, &pOldRTV, &pOldDSV );
	pd3dDevice->OMSetRenderTargets(1, &g_pRTVBack, pOldDSV);
	pd3dDevice->ClearRenderTargetView(g_pRTVBack, ClearColor);
	pd3dDevice->ClearDepthStencilView(pOldDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);

	D3DXMATRIX mProj, mView, mWorld, mWorldViewProj;
	mProj = *g_Camera.GetProjMatrix();
	mView = *g_Camera.GetViewMatrix();
	mWorld = g_mWorldScaling * (*g_MCamera.GetWorldMatrix());
	mWorldViewProj = mWorld * mView * mProj;
	g_pmWorldViewProj->SetMatrix((float*)& mWorldViewProj);

	D3D10_VIEWPORT VP;
	UINT cVPs = 1;
	pd3dDevice->RSGetViewports(&cVPs, &VP);

	//a. consts
	V((g_pEffect->GetVariableByName("v_orig")->AsVector())->SetFloatVector(g_pPlot->orig));
	D3DXVECTOR3 v_coordScale;
	for(int i = 0; i < 3; ++i)
		v_coordScale[i] = g_coordScale[i] / (float)g_schema->nScaleCur[i];
	V((g_pEffect->GetVariableByName("v_coordScale")->AsVector())->SetFloatVector(v_coordScale));
	V((g_pEffect->GetVariableByName("v_scaleRatio")->AsVector())->SetIntVector(g_schema->scaleRatioCur));
	//index of x,y,z in 1st texel; of measure in 2nd texel
	int xyzmAt[4] = {g_schema->showAt[0], g_schema->showAt[1], g_schema->showAt[2], g_schema->meaAt};
	V((g_pEffect->GetVariableByName("v_xyzmAt")->AsVector())->SetIntVector(xyzmAt));

	//b. records
	//gs sprite position for four triangle verts
	D3DXMATRIX matSpritePos( -g_spriteSize, g_spriteSize, 0, 0,		
		g_spriteSize, g_spriteSize, 0, 0,		
		-g_spriteSize, -g_spriteSize, 0, 0,		
		g_spriteSize, -g_spriteSize, 0, 0);
	D3DXMATRIX SPP = matSpritePos * *g_Camera.GetProjMatrix();
	pd3dDevice->IASetInputLayout(NULL);
	pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	V((g_pEffect->GetVariableByName("v_matSPP")->AsMatrix())->SetMatrix(SPP));
	V((g_pEffect->GetVariableByName("v_texParticle")->AsShaderResource())->SetResource(g_pParticleSRV));
	V((g_pEffect->GetVariableByName("v_lumi")->AsScalar())->SetFloat(g_lumi));
	assert((g_sampleCon & (g_sampleCon - 1)) == 0);
	assert((g_sampleFoc & (g_sampleFoc - 1)) == 0);
	V((g_pEffect->GetVariableByName("texTableDim")->AsShaderResource())->SetResource(g_pTable->pSRVDimData));
	V((g_pEffect->GetVariableByName("texTableMea")->AsShaderResource())->SetResource(g_pTable->pSRVMeaData));
	V((g_pEffect->GetVariableByName("texTableW")->AsScalar())->SetInt(g_pTable->texDataW));
	
	//c. draw billboard on focus, to Tmp
	V((g_pEffect->GetVariableByName("v_sampleRate")->AsScalar())->SetInt(g_sampleFoc));
	switch (g_schema->aggrType)
	{
	case AggrSum:
	case AggrCnt:
	case AggrAvg: //all 3 are the same
		g_pTechDrawPlot->GetPassByName("spritesSumB")->Apply(0);
		break;
	case AggrMax:
		g_pTechDrawPlot->GetPassByName("spritesMaxB")->Apply(0);
		break;
	case AggrMin:
		g_pTechDrawPlot->GetPassByName("spritesMinFloodMaxB")->Apply(0); //pre-flood
		pd3dDevice->Draw(g_pTable->nRec / g_sampleFoc, 0);
		g_pTechDrawPlot->GetPassByName("spritesMinB")->Apply(0);
		break;
	default:
		assert(0);
	}
	pd3dDevice->Draw(g_pTable->nRec / g_sampleFoc, 0);

	//2. new a 1x1 texture, CopySubresourceRegion to it
	GetCursorPos(&g_picked.pos);
	ScreenToClient(DXUTGetHWND(), &g_picked.pos);
	D3D10_BOX box = {g_picked.pos.x, g_picked.pos.y, 0, g_picked.pos.x + 1, g_picked.pos.y + 1, 1};
	pd3dDevice->CopySubresourceRegion(g_pTexStage1Pix, 0, 0, 0, 0, g_pTexBack, 0, &box);
	D3D10_MAPPED_TEXTURE2D mappedTex;
	g_pTexStage1Pix->Map(0, D3D10_MAP_READ, NULL, &mappedTex);
	float* pSrc = (float*)mappedTex.pData;
	memcpy(g_picked.pix, pSrc, 4 * sizeof(float));
	g_pTexStage1Pix->Unmap(0);

	//3. post process
	if (g_picked.pix.w > 0.9) //not bkgd, has alpha=1
	{
		switch (g_schema->aggrType)
		{
		case AggrSum:
			break;
		case AggrCnt:
			g_picked.pix = D3DXVECTOR4(g_picked.pix.w, g_picked.pix.w, g_picked.pix.w, g_picked.pix.w);
			break;
		case AggrAvg: //all 3 are the same
			g_picked.pix /= g_picked.pix.w;
			break;
		case AggrMax:
			break;
		case AggrMin:
			break;
		default:
			assert(0);
		}
		g_bPickSuccess = true;
	}

	// restore old render target and DS
	pd3dDevice->OMSetRenderTargets( 1, &pOldRTV, pOldDSV );
	SAFE_RELEASE( pOldRTV );
	SAFE_RELEASE( pOldDSV );
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	// If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }
	if(!g_bStartedRender) //first time: init context stencil
	{
		InitContextStencil(pd3dDevice);
		InitTexBack(pd3dDevice);
		g_bStartedRender = true;
	}
	if(g_changedAxis >= 0) //scatterplot's grid has changed
	{
		g_schema->OnShowAtUpdated(g_changedAxis);
		g_pPlot->UpdateGrid(g_changedAxis, g_schema, pd3dDevice);
		g_pPlot->UpdateDice(g_changedAxis, g_schema);
		g_changedAxis = -1; //reset
	}

	if(!g_bMiddleButtonDown)
		g_bPickSuccess = false;
	if(GetCapture() && g_bMiddleButtonDown && !g_bPickSuccess) //yk: currently borrow middle as "pick" rather than "light".
		Pick(pd3dDevice); //if success, set g_bPickSuccess as true
	
	float ClearColor[4] = { 0.0, 0.0, 0.0, 0.0 };
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	pd3dDevice->ClearRenderTargetView( pRTV, ClearColor);
	ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
	pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);

	D3DXMATRIX mProj, mView, mWorld, mWorldViewProj;

	//////////////////////////////////////////////////////////////////////////Focus
	mProj = *g_Camera.GetProjMatrix();
	mView = *g_Camera.GetViewMatrix();
	mWorld = g_mWorldScaling * (*g_MCamera.GetWorldMatrix());
	mWorldViewProj = mWorld * mView * mProj;
	g_pmWorldViewProj->SetMatrix((float*)& mWorldViewProj);
	DrawPlots(pd3dDevice, g_pTechDrawPlot, g_pTable, g_pPlot, g_schema, true);
	if(g_bDrawEnv)
	{
		DrawEnv(pd3dDevice, true);
		//g_pTechEnv->GetPassByIndex(1)->Apply(0);  //focus states
		//g_envmap.D3D10Render(&mWorldViewProj);
	}

	//////////////////////////////////////////////////////////////////////////context
	if(g_bDrawContext)
	{
		//viewport small
		D3D10_VIEWPORT VP;
		UINT cVPs = 1;
		pd3dDevice->RSGetViewports(&cVPs, &VP);
		const int conW = (int)VP.Width * cConSizeRatio;
		const int conH = (int)VP.Height * cConSizeRatio;
		D3D10_VIEWPORT VP1 = VP;
		VP1.TopLeftX = 0;
		VP1.TopLeftY = VP.Height - conH;
		VP1.Width = conW;
		VP1.Height = conH;
		pd3dDevice->RSSetViewports(1, &VP1);

		//draw boarder lines
		pd3dDevice->IASetInputLayout(g_pLayoutPos3);
		UINT stride = sizeof(Pos3f);
		UINT offset = 0;
		pd3dDevice->IASetVertexBuffers(0, 1, &g_pContextBorderVB, &stride, &offset);
		pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		g_pTechContextBorder->GetPassByIndex(0)->Apply(0);
		pd3dDevice->Draw(c_nVContextBorder, 0);
		
		D3DXMATRIX oldM = g_mWorldScaling * (*g_MCamera.GetWorldMatrix());
		D3DXMATRIX oldV = *g_Camera.GetViewMatrix();
		D3DXMATRIX oldP = *g_Camera.GetProjMatrix();
		const D3DXMATRIX oldMVP = oldM * oldV * oldP;
		
		//lookat
		D3DXVECTOR3 eye = *g_Camera.GetEyePt();
		D3DXVECTOR3 at = *g_Camera.GetLookAtPt();
		D3DXVECTOR3 eye1 = eye;
		D3DXVECTOR3 at1 = at;
		D3DXVECTOR3 tmp = at - eye;
		D3DXVec3Normalize(&eye1, &tmp);
		eye1 *= - g_conDist; //far enough to contain the whole world
		at1 = D3DXVECTOR3(0,0,0);
		g_Camera.SetViewParams(&eye1, &at1);

		//draw context
		mProj = *g_Camera.GetProjMatrix();
		mView = *g_Camera.GetViewMatrix();
		mWorld = g_mWorldScaling * (*g_MCamera.GetWorldMatrix());
		mWorldViewProj = mWorld * mView * mProj;
		g_pmWorldViewProj->SetMatrix((float*)& mWorldViewProj);
		DrawPlots(pd3dDevice, g_pTechDrawPlot, g_pTable, g_pPlot, g_schema, false);
		if(g_bDrawEnv)
		{
			DrawEnv(pd3dDevice, false);
			//g_pTechEnv->GetPassByIndex(0)->Apply(0);  //context states
			//g_envmap.D3D10Render(&mWorldViewProj);
		}
		//draw little focus: (mvp)-1 * MVP, where mvp are old
		D3DXMATRIX mT1, mT2, mT3; 
		D3DXMatrixInverse(&mT1, NULL, &oldMVP);
		mT2 = mT1 * mWorldViewProj;
		pd3dDevice->IASetInputLayout(g_pLayoutPos3);
		stride = sizeof(Pos3f);
		offset = 0;
		pd3dDevice->IASetVertexBuffers(0, 1, &g_pLittleFocusVB, &stride, &offset);
		pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		V((g_pEffect->GetVariableByName("v_zn")->AsScalar())->SetFloat(g_Camera.GetNearClip()));
		g_pmWorldViewProj->SetMatrix((float*)&mT2);
		g_pTechLittleFocus->GetPassByIndex(0)->Apply(0);
		pd3dDevice->Draw(c_nVContextBorder, 0);

		//recover viewport and lookat
		pd3dDevice->RSSetViewports(1, &VP);
		g_Camera.SetViewParams(&eye, &at);
	}
	
	//text
    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    DrawText(pd3dDevice);
	g_HUD.OnRender( fElapsedTime );
    if(g_bShowSetting)
	{
		g_SettingUI.OnRender( fElapsedTime );
	}
    DXUT_EndPerfEvent();

}

//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10SwapChainReleasing( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10ReleasingSwapChain();
	g_envmap.OnD3D10ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10DestroyDevice();
    g_D3DSettingsDlg.OnD3D10DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_RELEASE(g_pFont10);       
    SAFE_RELEASE(g_pEffect);
    SAFE_RELEASE(g_pSprite10);
    SAFE_DELETE(g_pTxtHelper);
	SAFE_DELETE(g_pPlot);
	SAFE_DELETE(g_pTable);
	SAFE_DELETE(g_schema);
	SAFE_RELEASE(g_pLayoutPos3Col3);
	SAFE_RELEASE(g_pContextLayout);
	SAFE_RELEASE(g_pLayoutPos3);
	SAFE_RELEASE(g_pContextBorderVB);
	SAFE_RELEASE(g_pBufPicked);
	SAFE_RELEASE(g_pStagePicked);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pParticleSRV);
	SAFE_RELEASE(g_pLittleFocusVB);
	g_envmap.OnD3D10DestroyDevice();
	SAFE_RELEASE(g_pEnvVB);
	SAFE_RELEASE(g_pEnvVBInd);
	SAFE_RELEASE(g_pDSEnableDS);
	SAFE_RELEASE(g_pSRVBack);
	SAFE_RELEASE(g_pRTVBack);
	SAFE_RELEASE(g_pTexBack);
	SAFE_RELEASE(g_pVBQuadPos3);
	SAFE_RELEASE(g_pLayoutPos3Tex2);
	SAFE_RELEASE(g_pVBQuadPos3Tex2);
	SAFE_RELEASE(g_pTexStage1Pix);
}
