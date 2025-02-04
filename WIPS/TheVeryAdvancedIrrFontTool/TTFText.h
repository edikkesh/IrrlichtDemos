#include "irrlicht.h"
using namespace irr;

#pragma comment(lib,"gdiplus.lib")
#include <gdiplus.h>
using namespace Gdiplus;
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
//http://msdn.microsoft.com/en-us/library/ms533843(VS.85).aspx
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

inline u32 getTextureSizeFromSurfaceSize(u32 size)
{
	u32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}

#define GetAValue(x)          (((x) >> 24) & 0xFF)
//#define GetRValue(x)          (((x) >> 16) & 0xFF)
//#define GetGValue(x)          (((x) >> 8) & 0xFF)
//#define GetBValue(x)          ((x) & 0xFF)

#define ARGB(a,r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))|(((DWORD)(BYTE)(a))<<24))
//#define RGBA( r, g, b, a ) ( (r<<24) | (g<<16) | (b<<8) | a )
//#define ARGB( a, r, g, b ) ( (a<<24) | (r<<16) | (g<<8) | b )
//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))


	class WT2IColor {
	public :
		WT2IColor() {myInit();};
		WT2IColor(COLORREF C, bool gradient = false, bool active = true) {myInit(); SetColor(C,active,gradient);};
		WT2IColor(COLORREF C1, COLORREF  C2, bool gradient = true, bool active = true) {myInit(); SetColorsVertical(C1,C2,active,gradient);};
		WT2IColor(COLORREF C1, COLORREF  C2, COLORREF  C3,COLORREF  C4, bool gradient = true, bool active = true) {myInit(); SetColors(C1,C2,C3,C4,active,gradient);};
		~WT2IColor() {};
		bool Active;
		bool Gradient;
		COLORREF Color;
		COLORREF Colors [4];
		void SetColor(COLORREF C, bool active = true, bool gradient = false) {Color = C; SetActive(active); SetGradient(gradient);};
		void SetColors(COLORREF C1,COLORREF  C2, int PosC1, bool active = true, bool gradient = true) {Colors[0] = C2; Colors[1] = C2; Colors[2] = C2; Colors[3] = C2; switch (abs(PosC1) % 4) {case 0: Colors[0] = C1; break; case 1: Colors[1] = C1; break; case 2: Colors[3] = C1; break; case 3: Colors[2] = C1; break; default : Colors[0] = C1;}; SetActive(active); SetGradient(gradient);};
		void SetColors(COLORREF C1,COLORREF  C2,COLORREF  C3,COLORREF  C4, bool active = true, bool gradient = true) {Colors[0] = C1; Colors[1] = C2; Colors[2] = C4; Colors[3] = C3; SetActive(active); SetGradient(gradient);};
		void SetColorsVertical(COLORREF C1,COLORREF  C2, bool active = true, bool gradient = true) {Colors[0] = C1; Colors[1] = C1; Colors[2] = C2; Colors[3] = C2; SetActive(active); SetGradient(gradient);};
		void SetColorsHorizontal(COLORREF C1,COLORREF  C2, bool active = true, bool gradient = true) {Colors[0] = C1; Colors[1] = C2; Colors[2] = C2; Colors[3] = C1; SetActive(active); SetGradient(gradient);};
		void SetColorsDiagonal(COLORREF C1,COLORREF  C2, bool active = true, bool gradient = true) {Colors[0] = C1; Colors[1] = C2; Colors[2] = C1; Colors[3] = C2; SetActive(active); SetGradient(gradient);};
		void SetActive(bool active = true) {Active = active;};
		void SetGradient(bool gradient = true) {Gradient = gradient;};
	private:
		void myInit () {SetActive(true); SetColor(RGB(255,255,255)); SetColors(RGB(255,0,0),RGB(0,0,255),RGB(0,0,0),RGB(0,255,0)), SetGradient(false);};
	};

	const u32 ROUNDTO_DEF = 0;//0 - Resize Image to 2^ (32,64,128,256....), 1 - NO resize
	const u32 ROUNDTO_TEXTURE_DEF = 1;//0 - Resize TEXTUR to 2^ (32,64,128,256....), 1 - NO resize
	class WT2I_FrameFormat
	{
	public :
		WT2I_FrameFormat() {myInit();};
		~WT2I_FrameFormat() {};
		bool Active;
		u32 roundX;//corner round
		u32 roundY;//corner round
		u32 distanceX;
		u32 distanceY;
		u32 size;
		WT2IColor Color;
		WT2IColor fillColor;
	private:
		void myInit () {roundX = 40; roundY = 40; distanceX = 2; distanceY = 2; size = 2; Color.SetActive(true); fillColor.SetActive(false);};
	} DefaultTextFrame;// = {false, 40,40,1,1,2};//RGB(255,0,255),RGB(128,128,255),RGB(255,255,0),RGB(0,255,0)

	const u8 WT2I_MAX_FRAMES = 5;
	class WT2I_Frames
	{
	public :
		WT2I_Frames() {myInit();};
		~WT2I_Frames() {};
//		bool Active;
		WT2I_FrameFormat Frame[WT2I_MAX_FRAMES];
	private:
		void myInit () {for (int i = 0;i < WT2I_MAX_FRAMES; i++) Frame[i].Active = false;};
	} DefaultFrames;// = {false, 40,40,1,1,2};//RGB(255,0,255),RGB(128,128,255),RGB(255,255,0),RGB(0,255,0)

	class WT2I_TextFormat
	{
	public :
		core::stringc fontname;		//"" : default "Arial"
		u32 textureHeight;			//0  : default = 256
		u32 textureWidth;			//0  : default = 256
		u32 fontSize;				//0  : default = 12
		u32 marginLeft;
		u32 marginRight;
		u32 marginTop;
		u32 marginBottom;
		bool font_bold;
		bool font_italic;
		bool font_alpha;
		bool font_antialias;
		bool font_underline;
		bool font_strikeout;
		bool text_multiline;
		bool text_left;
		bool text_center;
		bool text_right;
		bool text_top;
		bool text_vcenter;
		bool text_bottom;
		bool text_ellipses_no_textend;	//"..."
		bool text_ellipses_no_wordend;	//"..."
		bool text_ellipses_filepath;	//"xxx...xxx"
		bool text_no_wordbreak;
		bool text_autosize_texture_to_fit;				//standart window - resizing height to fit
		bool text_autosize_texture_to_fit_advanced;		//slowly
		bool text_texture_keep_aspect_ratio;			//if resizing
		COLORREF alpha_Color;
		u32 MAX_TEXTURE_SIZE;							//0  : default
		u32 ROUNDTO;
		u32 ROUNDTO_TEXTURE;
		WT2IColor Color;
		WT2IColor fontColor;
		WT2I_TextFormat() :
			textureHeight(0), textureWidth(0), fontSize(0), marginLeft(0), marginRight(0), marginTop(0), marginBottom(0), 
			font_bold(false), font_italic(false), font_alpha(false), font_antialias(false), font_underline(false), font_strikeout(false), 
			text_multiline(false), text_left(false), text_center(false), text_right(false), text_top(false), text_vcenter(false), text_bottom(false), 
			text_ellipses_no_textend(false), text_ellipses_no_wordend(false), text_ellipses_filepath(false), text_no_wordbreak(false), 
			text_autosize_texture_to_fit(false), text_autosize_texture_to_fit_advanced(false), text_texture_keep_aspect_ratio(false), 
			alpha_Color(0), MAX_TEXTURE_SIZE(0), ROUNDTO (ROUNDTO_DEF), ROUNDTO_TEXTURE (ROUNDTO_TEXTURE_DEF) 
			{myInit();};

		~WT2I_TextFormat() {};
	private:
		void myInit () {this->Color.SetColor(0);};
	} DefaultWinTextFormat;
	const u32 WinText2Image_MAX_TEXTURE_SIZE_DEFAULT = 2048;//or 8192;
	const u32 WinText2Image_MAX_TEXTURE_SIZE_MAXIMUM = 4096;//or 8192;




// Interpolates intermediate value at N
// X0 corresponds to N = 0 and X1 to N = 255
#define IPOL(X0, X1, N) ((X0) + ((X1) - (X0)) * N / 256)

const float xyStep[256] = {
		(float)0/(float)255,(float)1/(float)255,		(float)2/(float)255,(float)3/(float)255,		(float)4/(float)255,(float)5/(float)255,		(float)6/(float)255,(float)7/(float)255,		(float)8/(float)255,(float)9/(float)255,		(float)10/(float)255,(float)11/(float)255,		(float)12/(float)255,(float)13/(float)255,		(float)14/(float)255,(float)15/(float)255,
		(float)16/(float)255,(float)17/(float)255,		(float)18/(float)255,(float)19/(float)255,		(float)20/(float)255,(float)21/(float)255,		(float)22/(float)255,(float)23/(float)255,		(float)24/(float)255,(float)25/(float)255,		(float)26/(float)255,(float)27/(float)255,		(float)28/(float)255,(float)29/(float)255,		(float)30/(float)255,(float)31/(float)255,
		(float)32/(float)255,(float)33/(float)255,		(float)34/(float)255,(float)35/(float)255,		(float)36/(float)255,(float)37/(float)255,		(float)38/(float)255,(float)39/(float)255,		(float)40/(float)255,(float)41/(float)255,		(float)42/(float)255,(float)43/(float)255,		(float)44/(float)255,(float)45/(float)255,		(float)46/(float)255,(float)47/(float)255,
		(float)48/(float)255,(float)49/(float)255,		(float)50/(float)255,(float)51/(float)255,		(float)52/(float)255,(float)53/(float)255,		(float)54/(float)255,(float)55/(float)255,		(float)56/(float)255,(float)57/(float)255,		(float)58/(float)255,(float)59/(float)255,		(float)60/(float)255,(float)61/(float)255,		(float)62/(float)255,(float)63/(float)255,
		(float)64/(float)255,(float)65/(float)255,		(float)66/(float)255,(float)67/(float)255,		(float)68/(float)255,(float)69/(float)255,		(float)70/(float)255,(float)71/(float)255,		(float)72/(float)255,(float)73/(float)255,		(float)74/(float)255,(float)75/(float)255,		(float)76/(float)255,(float)77/(float)255,		(float)78/(float)255,(float)79/(float)255,
		(float)80/(float)255,(float)81/(float)255,		(float)82/(float)255,(float)83/(float)255,		(float)84/(float)255,(float)85/(float)255,		(float)86/(float)255,(float)87/(float)255,		(float)88/(float)255,(float)89/(float)255,		(float)90/(float)255,(float)91/(float)255,		(float)92/(float)255,(float)93/(float)255,		(float)94/(float)255,(float)95/(float)255,
		(float)96/(float)255,(float)97/(float)255,		(float)98/(float)255,(float)99/(float)255,		(float)100/(float)255,(float)101/(float)255,		(float)102/(float)255,(float)103/(float)255,		(float)104/(float)255,(float)105/(float)255,		(float)106/(float)255,(float)107/(float)255,		(float)108/(float)255,(float)109/(float)255,		(float)110/(float)255,(float)111/(float)255,
		(float)112/(float)255,(float)113/(float)255,		(float)114/(float)255,(float)115/(float)255,		(float)116/(float)255,(float)117/(float)255,		(float)118/(float)255,(float)119/(float)255,		(float)120/(float)255,(float)121/(float)255,		(float)122/(float)255,(float)123/(float)255,		(float)124/(float)255,(float)125/(float)255,		(float)126/(float)255,(float)127/(float)255,
		(float)128/(float)255,(float)129/(float)255,		(float)130/(float)255,(float)131/(float)255,		(float)132/(float)255,(float)133/(float)255,		(float)134/(float)255,(float)135/(float)255,		(float)136/(float)255,(float)137/(float)255,		(float)138/(float)255,(float)139/(float)255,		(float)140/(float)255,(float)141/(float)255,		(float)142/(float)255,(float)143/(float)255,
		(float)144/(float)255,(float)145/(float)255,		(float)146/(float)255,(float)147/(float)255,		(float)148/(float)255,(float)149/(float)255,		(float)150/(float)255,(float)151/(float)255,		(float)152/(float)255,(float)153/(float)255,		(float)154/(float)255,(float)155/(float)255,		(float)156/(float)255,(float)157/(float)255,		(float)158/(float)255,(float)159/(float)255,
		(float)160/(float)255,(float)161/(float)255,		(float)162/(float)255,(float)163/(float)255,		(float)164/(float)255,(float)165/(float)255,		(float)166/(float)255,(float)167/(float)255,		(float)168/(float)255,(float)169/(float)255,		(float)170/(float)255,(float)171/(float)255,		(float)172/(float)255,(float)173/(float)255,		(float)174/(float)255,(float)175/(float)255,
		(float)176/(float)255,(float)177/(float)255,		(float)178/(float)255,(float)179/(float)255,		(float)180/(float)255,(float)181/(float)255,		(float)182/(float)255,(float)183/(float)255,		(float)184/(float)255,(float)185/(float)255,		(float)186/(float)255,(float)187/(float)255,		(float)188/(float)255,(float)189/(float)255,		(float)190/(float)255,(float)191/(float)255,
		(float)192/(float)255,(float)193/(float)255,		(float)194/(float)255,(float)195/(float)255,		(float)196/(float)255,(float)197/(float)255,		(float)198/(float)255,(float)199/(float)255,		(float)200/(float)255,(float)201/(float)255,		(float)202/(float)255,(float)203/(float)255,		(float)204/(float)255,(float)205/(float)255,		(float)206/(float)255,(float)207/(float)255,
		(float)208/(float)255,(float)209/(float)255,		(float)210/(float)255,(float)211/(float)255,		(float)212/(float)255,(float)213/(float)255,		(float)214/(float)255,(float)215/(float)255,		(float)216/(float)255,(float)217/(float)255,		(float)218/(float)255,(float)219/(float)255,		(float)220/(float)255,(float)221/(float)255,		(float)222/(float)255,(float)223/(float)255,
		(float)224/(float)255,(float)225/(float)255,		(float)226/(float)255,(float)227/(float)255,		(float)228/(float)255,(float)229/(float)255,		(float)230/(float)255,(float)231/(float)255,		(float)232/(float)255,(float)233/(float)255,		(float)234/(float)255,(float)235/(float)255,		(float)236/(float)255,(float)237/(float)255,		(float)238/(float)255,(float)239/(float)255,
		(float)240/(float)255,(float)241/(float)255,		(float)242/(float)255,(float)243/(float)255,		(float)244/(float)255,(float)245/(float)255,		(float)246/(float)255,(float)247/(float)255,		(float)248/(float)255,(float)249/(float)255,		(float)250/(float)255,(float)251/(float)255,		(float)252/(float)255,(float)253/(float)255,		(float)254/(float)255,(float)255/(float)255};

// Fills the rectangle 'rect' in graded colours
void GradientFill(HDC hdc, RECT rect, COLORREF * color_array)
{
//    GradientFillRectVert(hdc,&rect,color_array[0],color_array[2]); return;

	LONG Height = rect.bottom - rect.top;
	LONG Width  = rect.right - rect.left;
  //COLORREF * color_array = new COLORREF[4];
//  color_array[3] = FromColor; color_array[2] = FromColor; color_array[1] = ToColor; color_array[0] = ToColor;
//  color_array[0] = FromColor; color_array[1] = FromColor; color_array[2] = ToColor; color_array[3] = ToColor;
  //color_array[0] = FromColor; color_array[2] = FromColor; color_array[1] = ToColor; color_array[3] = ToColor;
//  color_array[0] = RGB(255,0,0); color_array[1] = RGB(0,255,0); color_array[2] = RGB(0,0,255); color_array[3] = RGB(0,0,0);
//  color_array[3] = RGB(255,0,0); color_array[2] = RGB(0,255,0); color_array[1] = RGB(0,0,255); color_array[0] = RGB(0,0,0);

  LONG toYArr[256];
  for (int y=0; y<256; y++) 
	  {
		  toYArr[y] = LONG ( xyStep[y]*Height ) + rect.top;
		  if (toYArr[y] > rect.bottom) 
			  toYArr[y] = rect.bottom;
	  }

  // X loop starts
  for (int iX = 0, X = rect.left; iX < 256; iX++)
  {
    // Calculates end colours of the band in Y direction
    int RGBColor[3][2] = {
      {IPOL(GetRValue(color_array[0]), GetRValue(color_array[1]), iX),
        IPOL(GetRValue(color_array[3]), GetRValue(color_array[2]), iX)
      },
      {IPOL(GetGValue(color_array[0]),  GetGValue(color_array[1]), iX),
        IPOL(GetGValue(color_array[3]), GetGValue(color_array[2]), iX)
      },
      {IPOL(GetBValue(color_array[0]), GetBValue(color_array[1]), iX),
        IPOL(GetBValue(color_array[3]), GetBValue(color_array[2]), iX)
      }
    };
	LONG toX = LONG ( xyStep[iX]*Width ) + rect.left;
    // Y loop starts
    for (int iY = 0, Y = rect.top; iY < 256; iY++)
    {
      // Calculates the colour of the rectangular band
      COLORREF Color = RGB(IPOL(RGBColor[0][0], RGBColor[0][1], iY),
        IPOL(RGBColor[1][0], RGBColor[1][1],  iY),
        IPOL(RGBColor[2][0], RGBColor[2][1], iY));
      // Creates the brush to fill the rectangle
      HBRUSH hBrush = CreateSolidBrush(Color);
      // Paints the rectangular band with the brush
	  LONG toY = toYArr[iY];
      RECT Rect = {X, Y,toX,toY};
      FillRect(hdc, &Rect, hBrush);
      // Deletes the brush
      DeleteObject(hBrush);
      Y = toY;
//	  if (Y > rect.bottom)
//        Y = rect.bottom;
    }
    // Updates X value of the rectangle
//    X = LONG((float)X + xStep);
    X = toX;
    if (X > rect.right)
      X = rect.right;
  }
//  rect.right -= (rect.right - rect.left) / 2;
//  GradientFillRectVert(hdc,&rect,FromColor,ToColor); return;
}


void GradientFill(Gdiplus::Graphics * gr, RectF rect, COLORREF * color_array)
{
//  GradientFillRectVert(hdc,&rect,color_array[0],color_array[2]); return;

//	Gdiplus::Graphics * gr = g;
//   SolidBrush  brush(Color(128, 255, 0, 255));
//	g->FillRectangle(0, 0, 0,0,0);
//	*g.IsVisible;

//	float Height = rect.bottom - rect.top;
//	float Width  = rect.right - rect.left;
  //COLORREF * color_array = new COLORREF[4];
//  color_array[3] = FromColor; color_array[2] = FromColor; color_array[1] = ToColor; color_array[0] = ToColor;
//  color_array[0] = FromColor; color_array[1] = FromColor; color_array[2] = ToColor; color_array[3] = ToColor;
  //color_array[0] = FromColor; color_array[2] = FromColor; color_array[1] = ToColor; color_array[3] = ToColor;
//  color_array[0] = RGB(255,0,0); color_array[1] = RGB(0,255,0); color_array[2] = RGB(0,0,255); color_array[3] = RGB(0,0,0);
//  color_array[3] = RGB(255,0,0); color_array[2] = RGB(0,255,0); color_array[1] = RGB(0,0,255); color_array[0] = RGB(0,0,0);
//	COLORREF A = ARGB(255,0,0,0);
//  color_array[3] |= ARGB(255,0,0,0); 
// color_array[2] |= ARGB(200,0,0,0); 
//  color_array[1] |= ARGB(155,0,0,0); 
//  color_array[0] |= ARGB(55,0,0,0);

//  color_array[3] |= ARGB(255,0,0,0); 
//  color_array[2] |= ARGB(255,0,0,0); 
//  color_array[1] |= ARGB(255,0,0,0); 
//  color_array[0] |= ARGB(255,0,0,0);

	

   SolidBrush  brush (Color(255,255,255,255));
  float toYArr[256];
  for (int y=0; y<256; y++) 
	  {
		  toYArr[y] = (xyStep[y]*rect.Height) + rect.Y;
		  if (toYArr[y] > (rect.Y+rect.Height)) 
			  toYArr[y] = (rect.Y+rect.Height);
	  }

  // X loop starts
  float X = rect.X;
  for (int iX = 0; iX < 256; iX++)
  {
    // Calculates end colours of the band in Y direction
    int RGBColor[4][2] = {
      {IPOL(GetRValue(color_array[0]), GetRValue(color_array[1]), iX),
        IPOL(GetRValue(color_array[3]), GetRValue(color_array[2]), iX)
      },
      {IPOL(GetGValue(color_array[0]),  GetGValue(color_array[1]), iX),
        IPOL(GetGValue(color_array[3]), GetGValue(color_array[2]), iX)
      },
      {IPOL(GetBValue(color_array[0]), GetBValue(color_array[1]), iX),
        IPOL(GetBValue(color_array[3]), GetBValue(color_array[2]), iX)
      },
      {IPOL(GetAValue(color_array[0]), GetAValue(color_array[1]), iX),
        IPOL(GetAValue(color_array[3]), GetAValue(color_array[2]), iX)
      }
    };
	float toX = ( xyStep[iX]*rect.Width ) + rect.X;
    // Y loop starts
	float Y = rect.Y;
    for (int iY = 0; iY < 256; iY++)
    {
      // Calculates the colour of the rectangular band
//      COLORREF C = RGB(IPOL(RGBColor[0][0], RGBColor[0][1], iY),
//        IPOL(RGBColor[1][0], RGBColor[1][1],  iY),
//        IPOL(RGBColor[2][0], RGBColor[2][1], iY));
      u8 r = IPOL(RGBColor[0][0], RGBColor[0][1], iY);
	  u8 g = IPOL(RGBColor[1][0], RGBColor[1][1],  iY);
	  u8 b = IPOL(RGBColor[2][0], RGBColor[2][1], iY);
	  u8 a = IPOL(RGBColor[3][0], RGBColor[3][1], iY);
//      u8 r = GetRValue(C);
//	  u8 g = GetGValue(C);
//	  u8 b = GetBValue(C);


	  float toY = toYArr[iY];
//      RECT Rect = {X, Y,toX,toY};

//	   SolidBrush  brush(C);
	   SolidBrush  brush(Color(a, r, g, b));
//	  brush.SetColor(Color(200,(u8) GetRValue(C),(u8) GetGValue(C),(u8) GetBValue(C)));
	   gr->FillRectangle(&brush, X, Y,toX-X,toY-Y);
	   
//	   *g-

      // Creates the brush to fill the rectangle
//      HBRUSH hBrush = CreateSolidBrush(Color);
      // Paints the rectangular band with the brush
//      FillRect(hdc, &Rect, hBrush);
      // Deletes the brush
//      DeleteObject(hBrush);

      Y = toY;
//	  if (Y > rect.bottom)
//        Y = rect.bottom;
    }
    // Updates X value of the rectangle
//    X = LONG((float)X + xStep);
    X = toX;
	if (X > rect.X + rect.Width)
      X = rect.X + rect.Width;
  }
//  rect.right -= (rect.right - rect.left) / 2;
//  GradientFillRectVert(hdc,&rect,FromColor,ToColor); return;
}


// Fills the region 'rgn' in graded colours
void GradientFill(HDC hdc, HRGN rgn,  COLORREF * color_array)
//								COLORREF FromColor,
//								COLORREF ToColor)
{
  // Creates memory DC
  HDC hMemDC = CreateCompatibleDC(hdc);
  if (hMemDC)  // Memory DC creation successful
  {
    // Gets bounding rectangle of region
    RECT rectRgn;
    GetRgnBox(rgn, &rectRgn);
    // Left top point of applying mask
    int X = rectRgn.left, Y = rectRgn.top;
    // Size of mask
    int Width = rectRgn.right - X, Height = rectRgn.bottom - Y;
    // Creates bitmap for the mask
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Width,Height);
    if (hBitmap)  // Bitmap created successfully
    {
      // Selects bitmap in memory DC
      HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);
      // Prepares gradient filled mask and applies to output DC
      OffsetRect(&rectRgn, -rectRgn.left, -rectRgn.top);
	  GradientFill(hMemDC, rectRgn, color_array);
      BitBlt(hdc, X, Y, Width, Height, hMemDC, 0, 0, SRCINVERT);
      // Displays region in black in output DC
      FillRgn(hdc, rgn, (HBRUSH) GetStockObject(BLACK_BRUSH));
      // Applies mask to output DC again
      BitBlt(hdc, X, Y, Width, Height, hMemDC, 0, 0, SRCINVERT);
      // De-selects bitmap from memory DC
      SelectObject(hMemDC, hOldBitmap);
      // Deletes bitmap
      DeleteObject(hBitmap);
    }
    // Deletes memory DC
    DeleteDC(hMemDC);
  }
}

// Fills the text 'text' in graded colours at (x, y)
void GradientFill(HDC hdc, int x, int y, LPCWSTR text,  COLORREF * color_array)
{
  // Creates memory DC
  HDC hMemDC = CreateCompatibleDC(hdc);
  if (hMemDC)          // Memory DC creation successful
  {
    // Gets size of the string
    SIZE Size;
    GetTextExtentPoint32(hdc, text, lstrlen(text), &Size);
    // Creates bounding rectangle of the text
    RECT rectText = {x, y, x + Size.cx, y + Size.cy};
    RECT rect = rectText;
    // Creates bitmap for the mask
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);
    if (hBitmap)      // Bitmap created successfully
    {
      // Selects bitmap in memory DC
      HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);
      // Prepares gradient filled mask and applies to output DC
      OffsetRect(&rectText, -rectText.left, -rectText.top);
	  GradientFill(hMemDC, rectText, color_array);
      BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCINVERT);
      // Displays text in black colour in output DC
      // Background mode is transparent
      COLORREF TextColor = SetTextColor(hdc, RGB(0, 0, 0));
      int BkMode = SetBkMode(hdc, TRANSPARENT);
      TextOut(hdc, x, y, text, lstrlen(text));
      SetBkMode(hdc, BkMode);
      SetTextColor(hdc, TextColor);
      // Applies mask to output DC again
      BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCINVERT);
      // De-selects bitmap from memory DC
      SelectObject(hMemDC, hOldBitmap);
      // Deletes bitmap
      DeleteObject(hBitmap);
    }
    // Deletes memory DC
    DeleteDC(hMemDC);
  }
}

//Old 
// Fills the text 'text' in graded colours at (x, y)
void GradientFill_Old(HDC hdc, RECT rectText, LPCWSTR text, UINT text_format,  COLORREF * color_array)

{
  // Creates memory DC
  HDC hMemDC = CreateCompatibleDC(hdc);
  if (hMemDC)          // Memory DC creation successful
  {
    // Gets size of the string
	SIZE Size = {rectText.right - rectText.left,rectText.bottom - rectText.top};
	int x = rectText.left;
	int y = rectText.top;
    RECT rect = rectText;
    // Creates bitmap for the mask
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);
    if (hBitmap)      // Bitmap created successfully
    {
      // Selects bitmap in memory DC
      HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);
      // Prepares gradient filled mask and applies to output DC
      OffsetRect(&rectText, -rectText.left, -rectText.top);
	  GradientFill(hMemDC, rectText, color_array);
      BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCINVERT);
      // Displays text in black colour in output DC
      // Background mode is transparent
      COLORREF TextColor = SetTextColor(hdc, RGB(0, 0, 0));
      int BkMode = SetBkMode(hdc, TRANSPARENT);
	  DrawTextW(hdc, text, lstrlen(text), &rect, text_format);
//      TextOut(hdc, x, y, text, lstrlen(text));
      SetBkMode(hdc, BkMode);
      SetTextColor(hdc, TextColor);
      // Applies mask to output DC again
      BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCINVERT);
      // De-selects bitmap from memory DC
      SelectObject(hMemDC, hOldBitmap);
      // Deletes bitmap
      DeleteObject(hBitmap);
    }
    // Deletes memory DC
    DeleteDC(hMemDC);
  }
}


void GradientFill(HDC hdc, RECT rectText, LPCWSTR text, UINT text_format,  COLORREF * color_array)
{
  // Creates memory DC
  HDC hMemDC = CreateCompatibleDC(hdc);
  HDC hMemTempDC = CreateCompatibleDC(hdc);
  if (hMemDC && hMemTempDC)          // Memory DC creation successful
  {
	SIZE Size = {rectText.right - rectText.left,rectText.bottom - rectText.top};
	int x = rectText.left;
	int y = rectText.top;
    RECT rect = rectText;
    // Creates bitmap for the mask
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);
    HBITMAP hBitmapTemp = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);

    if (hBitmap && hBitmapTemp)      // Bitmap created successfully
    {
      // Selects bitmap in memory DC
      HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);
      HBITMAP hOldBitmapTemp = (HBITMAP) SelectObject(hMemTempDC, hBitmapTemp);
      // Prepares gradient filled mask and applies to output DC
      OffsetRect(&rectText, -rectText.left, -rectText.top);
      GradientFill(hMemDC, rectText, color_array);

	  //Copy BACKGRAUND
	  BitBlt(hMemTempDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCCOPY);
//	  GradientFill(hMemTempDC, rectText, RGB(255,0,255), RGB(255,0,0));
	  //Clear BACKGRUND
	  BitBlt(hdc, x, y, Size.cx, Size.cy, 0, 0, 0, BLACKNESS);

//	  BitBlt(hdc, x, y, Size.cx, Size.cy, 0, 0, 0, WHITENESS);

	  // Displays text in white colour in output DC
      // Background mode is transparent
      COLORREF TextColor = SetTextColor(hdc, RGB(255, 255, 255));
//      COLORREF TextColor = SetTextColor(hdc, RGB(0, 0, 0));
	  int BkMode = SetBkMode(hdc, TRANSPARENT);
	  DrawTextW(hdc, text, lstrlen(text), &rect, text_format);
//	  BitBlt(hdc, x, y, Size.cx, Size.cy, 0, 0, 0, DSTINVERT);

//*
	  //Make Gradient TEXT
	  BitBlt(hMemDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCCOPY);

////	  MakeGrayScale(hMemDC,hBitmap);
//	  if (MakeGrayScale(hMemDC,hBitmap)) std::cout << "Gray OK" << std::endl; else std::cout << "Gray fails" << std::endl;

	  //Place Gradient TEXT
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCCOPY);
/**/

	  GradientFill(hMemDC, rectText, color_array);
	  //Place Gradient TEXT
//	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCCOPY);
/**/

//      GradientFill(hMemDC, rectText, color_array);

/*
	  SelectObject(hMemBWDC, font);
	  SetTextAlign(hMemBWDC,TA_LEFT | TA_TOP | TA_NOUPDATECP);

	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, 0, 0, 0, BLACKNESS);
      SetTextColor(hMemBWDC, RGB(255, 255, 255));
//	  SetBkMode(hMemBWDC, TRANSPARENT);
	  DrawTextW(hMemBWDC, text, lstrlen(text), &rectText, text_format);

//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, 0, 0, 0, BLACKNESS);
//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCPAINT);
//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, 0, 0, 0, BLACKNESS);
//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, 0, 0, 0, WHITENESS);

//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCCOPY);
//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCPAINT);
//	  BitBlt(hMemBWDC, 0, 0, Size.cx, Size.cy, hdc, x, y, MERGEPAINT);
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemBWDC, 0, 0, SRCCOPY);
/**/
//*
	  //Make Gradient TEXT
	  BitBlt(hMemDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCAND);

	  //Return BACKGRAUND Masked with TEXT 
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemTempDC, 0, 0, SRCERASE);
/*
	  //sometimes better result (~white BG) - Return BACKGRAUND and draw TEXT with black color
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemTempDC, 0, 0, SRCCOPY);
      SetTextColor(hdc, RGB(0,0,0));
	  DrawTextW(hdc, text, lstrlen(text), &rect, text_format);
/**/
//*
	  //Place Gradient TEXT
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCPAINT);
/**/
      SetBkMode(hdc, BkMode);
      SetTextColor(hdc, TextColor);

      // De-selects bitmap from memory DC
      SelectObject(hMemDC, hOldBitmap);
	  SelectObject(hMemTempDC, hOldBitmapTemp);
      // Deletes bitmap
      DeleteObject(hBitmap);
	  DeleteObject(hBitmapTemp);
    }
    // Deletes memory DC
    DeleteDC(hMemDC);
    DeleteDC(hMemTempDC);
  }
}

// Gradient Raund Rect
void GradientFill(HDC hdc, RECT rectText, int roundX, int roundY, int size, COLORREF * color_array)
{
  // Creates memory DC
  HDC hMemDC = CreateCompatibleDC(hdc);
  HDC hMemTempDC = CreateCompatibleDC(hdc);
  if (hMemDC && hMemTempDC)          // Memory DC creation successful
  {
	int s = (size / 2)+1;
    RECT rect = rectText;
	rectText.top -= s;
	rectText.left -= s;
	rectText.right += s;
	rectText.bottom += s;
//	InflateRect
	SIZE Size = {rectText.right - rectText.left,rectText.bottom - rectText.top};
	int x = rectText.left;
	int y = rectText.top;
    // Creates bitmap for the mask
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);
    HBITMAP hBitmapTemp = CreateCompatibleBitmap(hdc, Size.cx, Size.cy);

    if (hBitmap && hBitmapTemp)      // Bitmap created successfully
    {
      // Selects bitmap in memory DC
      HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);
      HBITMAP hOldBitmapTemp = (HBITMAP) SelectObject(hMemTempDC, hBitmapTemp);
      // Prepares gradient filled mask and applies to output DC
      OffsetRect(&rectText, -rectText.left, -rectText.top);
      GradientFill(hMemDC, rectText, color_array);

	  //Copy BACKGRAUND
	  BitBlt(hMemTempDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCCOPY);
//	  GradientFill(hMemTempDC, rectText, RGB(255,0,255), RGB(255,0,0));
	  //Clear BACKGRUND
	  BitBlt(hdc, x, y, Size.cx, Size.cy, 0, 0, 0, BLACKNESS);

		//init Border pen
		// Create a geometric pen.
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = RGB(255,255,255);//set color white
		HPEN pen2 = ExtCreatePen(PS_GEOMETRIC|PS_ENDCAP_ROUND, size, &logBrush, 0, 0);

		//init Fill brush
		LOGBRUSH lbrush2;
		lbrush2.lbColor = RGB(0,0,0);
		lbrush2.lbHatch = 0;
		lbrush2.lbStyle = BS_NULL;
		HBRUSH brush2 = CreateBrushIndirect(&lbrush2);

		//select brush and pen
		HGDIOBJ oldbrush	= SelectObject(hdc, brush2);
		HGDIOBJ oldpen	= SelectObject(hdc, pen2);

		//draw WHITE text
		RoundRect(hdc,rect.left,rect.top,rect.right,rect.bottom,roundX,roundY);

		SelectObject(hdc, oldbrush);
		SelectObject(hdc, oldpen);

		DeleteObject(brush2);
		DeleteObject(pen2);

	  // Displays text in white colour in output DC
      // Background mode is transparent
      COLORREF TextColor = SetTextColor(hdc, RGB(255, 255, 255));
	  int BkMode = SetBkMode(hdc, TRANSPARENT);
//	  DrawTextW(hdc, text, lstrlen(text), &rect, text_format);

	  //Make Gradient TEXT
	  BitBlt(hMemDC, 0, 0, Size.cx, Size.cy, hdc, x, y, SRCAND);

	  //Return BACKGRAUND Masked with TEXT 
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemTempDC, 0, 0, SRCERASE);
/*
	  //sometimes better for dark backgraund result - Return BACKGRAUND and draw TEXT with black color
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemTempDC, 0, 0, SRCCOPY);
      SetTextColor(hdc, RGB(0,0,0));
	  DrawTextW(hdc, text, lstrlen(text), &rect, text_format);
/**/
	  //Place Gradient TEXT
	  BitBlt(hdc, x, y, Size.cx, Size.cy, hMemDC, 0, 0, SRCPAINT);

      SetBkMode(hdc, BkMode);
      SetTextColor(hdc, TextColor);

      // De-selects bitmap from memory DC
      SelectObject(hMemDC, hOldBitmap);
	  SelectObject(hMemTempDC, hOldBitmapTemp);
      // Deletes bitmap
      DeleteObject(hBitmap);
	  DeleteObject(hBitmapTemp);
    }
    // Deletes memory DC
    DeleteDC(hMemDC);
    DeleteDC(hMemTempDC);
  }
}

	void DrawFrame(HDC hdc, const RECT &rect, WT2I_FrameFormat Frame)
	{
	if (!Frame.Active) return;
	if (!(Frame.fillColor.Active || Frame.Color.Active)) return;
		{
		//init Frame Border pen
		// Create a geometric pen.
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = Frame.Color.Color;
		HPEN pen2 = ((Frame.size <= 0) || !Frame.Color.Active || Frame.Color.Gradient) ? CreatePen(PS_NULL, 0, 0) : ExtCreatePen(PS_GEOMETRIC|PS_ENDCAP_ROUND, Frame.size, &logBrush, 0, 0);

		//init Frame Fill brush
		LOGBRUSH lbrush2;
		lbrush2.lbColor = Frame.fillColor.Color;
		lbrush2.lbHatch = 0;
		if (Frame.fillColor.Active && !Frame.fillColor.Gradient)
			lbrush2.lbStyle = BS_SOLID;
		else 
			lbrush2.lbStyle = BS_NULL;
		HBRUSH brush2 = CreateBrushIndirect(&lbrush2);

		//init Frame rect
		RECT frame_rect;
		frame_rect.left   = rect.left-(Frame.distanceX+Frame.size/2);
		frame_rect.top    = rect.top-(Frame.distanceY+Frame.size/2);
		frame_rect.right  = rect.right+(Frame.distanceX+Frame.size/2)+1;
		frame_rect.bottom = rect.bottom+(Frame.distanceY+Frame.size/2)+1;

		//select brush and pen
		HGDIOBJ oldbrush	= SelectObject(hdc, brush2);
		HGDIOBJ oldpen	= SelectObject(hdc, pen2);

		//Draw Frame Fill
		if (Frame.fillColor.Active)
			{
				HRGN hClipRgn;
				//Call InflateRect(ClientArea, -5, -5) - promenia razmera na rect

				//' Create and selection clipping region for fill
				hClipRgn = CreateRoundRectRgn(frame_rect.left, frame_rect.top, 
						frame_rect.right+1, frame_rect.bottom+1, Frame.roundX,Frame.roundY);
						SelectClipRgn(hdc, hClipRgn);
						
				if (Frame.fillColor.Gradient)
					GradientFill(hdc,frame_rect,Frame.fillColor.Colors);
				else
					Rectangle(hdc,frame_rect.left,frame_rect.top,frame_rect.right,frame_rect.bottom);
				SelectClipRgn(hdc,0);
				DeleteObject(hClipRgn);
			}


		//Draw Frame Border
		if (Frame.size && Frame.Color.Active)
			if (Frame.Color.Gradient)
				GradientFill(hdc,frame_rect,Frame.roundX,Frame.roundY,Frame.size,Frame.Color.Colors);
			else //this draws border and fill 
				RoundRect(hdc,frame_rect.left,frame_rect.top,frame_rect.right,frame_rect.bottom,Frame.roundX,Frame.roundY);

		SelectObject(hdc, oldbrush);
		SelectObject(hdc, oldpen);

		DeleteObject(brush2);
		DeleteObject(pen2);
	}
}


	//
	// WinText2Image - Creates IImage from Text
	//
	video::IImage* WinText2Image_old(irr::IrrlichtDevice* Device, const wchar_t* text, const WT2I_TextFormat * TextFormat = &DefaultWinTextFormat,
		WT2I_FrameFormat * TextFrame = &DefaultTextFrame, WT2I_Frames * TextureFrames = &DefaultFrames)
	{
		if (!Device)
			{return 0;}
		if (!TextFormat)
			{return 0;}

		//
		// Init Text Format
		//
		u32 texHeight	 = TextFormat->textureHeight	== 0 ? 256 : TextFormat->textureHeight;
		u32 texWidth	 = TextFormat->textureWidth		== 0 ? 256 : TextFormat->textureWidth;
		u32 ROUNDTO		 = TextFormat->ROUNDTO;
		u32 ROUNDTO_TEXTURE = TextFormat->ROUNDTO_TEXTURE;
		if (ROUNDTO < 0) ROUNDTO = 0;
		if (!ROUNDTO)
			{
				texWidth = getTextureSizeFromSurfaceSize(texWidth);
				texHeight = getTextureSizeFromSurfaceSize(texHeight);
			}
		else
			{
			if (texWidth % ROUNDTO) texWidth += (ROUNDTO - (texWidth % ROUNDTO));
			if (texHeight % ROUNDTO) texHeight += (ROUNDTO - (texHeight % ROUNDTO));
			}
		u32 WinText2Image_MAX_TEXTURE_SIZE
			= TextFormat->MAX_TEXTURE_SIZE < 16 ? WinText2Image_MAX_TEXTURE_SIZE_DEFAULT : TextFormat->MAX_TEXTURE_SIZE;
		if (WinText2Image_MAX_TEXTURE_SIZE > WinText2Image_MAX_TEXTURE_SIZE_MAXIMUM)
			WinText2Image_MAX_TEXTURE_SIZE = WinText2Image_MAX_TEXTURE_SIZE_MAXIMUM;
		f32 texAspect    = (f32)((f32)texWidth / (f32)texHeight); if (texAspect == 0) texAspect = 1;
		u32 marginLeft	 = 0;
		u32 marginRight	 = 0;
		u32 marginTop	 = 0;
		u32 marginBottom = 0;
		bool text_multiline					= TextFormat->text_multiline;
		bool text_ellipses_no_textend		= TextFormat->text_ellipses_no_textend; //"..."
		bool text_ellipses_no_wordend		= TextFormat->text_ellipses_no_wordend; //"..."
		bool text_ellipses_filepath			= TextFormat->text_ellipses_filepath; //"xxx...xxx"
		bool text_no_wordbreak				= TextFormat->text_no_wordbreak;
		bool text_autosize_texture_to_fit	= TextFormat->text_autosize_texture_to_fit;
		bool text_autosize_texture_to_fit_advanced	= TextFormat->text_autosize_texture_to_fit_advanced;
		if (text_autosize_texture_to_fit_advanced) 
			text_autosize_texture_to_fit = true;
		bool text_texture_keep_aspect_ratio			= TextFormat->text_texture_keep_aspect_ratio;
		bool text_left				= TextFormat->text_left;
		bool text_center			= TextFormat->text_center;
		bool text_right				= TextFormat->text_right;
		bool text_top				= TextFormat->text_top;
		bool text_vcenter			= TextFormat->text_vcenter;
		bool text_bottom			= TextFormat->text_bottom;
		if (text_center) //Set horizontal alignment priority to center, right, left // default is left
			{
				text_left  = false;
				text_right = false;
			}
		else
			if (text_right)
				text_left = false;
			else
				text_left = true;
		if (text_vcenter) //Set vertical alignment priority to vcenter, bottom, top // default is top
			{
			text_top    = false;
			text_bottom = false;
			}
		else
			if (text_bottom)
				text_top = false;
			else
				text_top = true;
		if (TextFormat->marginLeft + TextFormat->marginRight < texWidth)
			{
				marginLeft	 = TextFormat->marginLeft;
				marginRight	 = TextFormat->marginRight;
			}
		if (TextFormat->marginTop  + TextFormat->marginBottom < texHeight)
			{
				marginTop	 = TextFormat->marginTop;
				marginBottom = TextFormat->marginBottom;
			}
		u32 BasemarginLeft	 = marginLeft;
		u32 BasemarginRight	 = marginRight;
		u32 BasemarginTop	 = marginTop;
		u32 BasemarginBottom = marginBottom;

		//
		// Init Text Frame
		//
		WT2I_FrameFormat Frame;
		Frame.Active = false;
		if (TextFrame && TextFrame->Active) 
			{
				Frame = *TextFrame;
				if (Frame.size > 20) Frame.size = 20;
				if (marginLeft + marginRight + 2*Frame.distanceX < texWidth)
					{
						marginLeft  += Frame.distanceX;
						marginRight += Frame.distanceX;
					}
				else
					Frame.distanceX = 0;
				if (marginTop + marginBottom + 2*Frame.distanceY < texHeight)
					{
						marginTop    += Frame.distanceY;
						marginBottom += Frame.distanceY;
					}
				else
					Frame.distanceY = 0;
				if ((marginLeft + marginRight + 2*Frame.size < texWidth) &&
					(marginTop + marginBottom + 2*Frame.size < texHeight))
					{
						marginLeft   += Frame.size;
						marginRight  += Frame.size;
						marginTop    += Frame.size;
						marginBottom += Frame.size;
					}
				else
					Frame.size = 0;
			}
		//
		// Init Texture Frames
		//
		WT2I_Frames Frames;
		if (TextureFrames) 
			Frames = *TextureFrames;
		for (int i = 0;i < WT2I_MAX_FRAMES; i++)
			if (Frames.Frame[i].Active) 
			{
				if (Frames.Frame[i].size > 20) Frames.Frame[i].size = 20;
				if (marginLeft + marginRight + 2*Frames.Frame[i].distanceX < texWidth)
					{
						marginLeft  += Frames.Frame[i].distanceX;
						marginRight += Frames.Frame[i].distanceX;
					}
				else
					Frames.Frame[i].distanceX = 0;
				if (marginTop + marginBottom + 2*Frames.Frame[i].distanceY < texHeight)
					{
						marginTop    += Frames.Frame[i].distanceY;
						marginBottom += Frames.Frame[i].distanceY;
					}
				else
					Frames.Frame[i].distanceY = 0;
				if ((marginLeft + marginRight + 2*Frames.Frame[i].size < texWidth) &&
					(marginTop + marginBottom + 2*Frames.Frame[i].size < texHeight))
					{
						marginLeft   += Frames.Frame[i].size;
						marginRight  += Frames.Frame[i].size;
						marginTop    += Frames.Frame[i].size;
						marginBottom += Frames.Frame[i].size;
					}
				else
					{
						Frames.Frame[i].size = 0;
					}
			}

		core::stringw Text = text;
		Text.trim();

#ifndef _IRR_WINDOWS_
		Device->getLogger()->log("WinText2Image: For Windows Only");
		return 0;
	}
#else
		//
		// init display context
		//
		HDC dc = CreateDC(L"DISPLAY", L"DISPLAY", 0 ,0 );
		if (!dc)
			{
				Device->getLogger()->log("WinText2Image: can't create display context (DC)");
				return 0;
			}
		
		
		//
		// init FontName
		//
		core::stringw fontn = TextFormat->fontname;
		if (fontn.size() == 0) //you can just type 0 for fontname
			fontn = "Arial";

		//
		// init FontSize
		//
		s32 fontSize = TextFormat->fontSize;
		if (fontSize <= 0) //you can just type 0 for fontsize
			fontSize = 12;

		bool UseAlphaChannel = TextFormat->font_alpha;
		u32 currentImage = 0;

		//
		// create the font
		//
		HFONT font = CreateFontW(
			-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72), 0,
			0,0,
			TextFormat->font_bold ? FW_BOLD : 0,
			TextFormat->font_italic, TextFormat->font_underline, TextFormat->font_strikeout, DEFAULT_CHARSET, 0,0,
			TextFormat->font_antialias ? ANTIALIASED_QUALITY : 0,
			0, fontn.c_str() );

		if (!font)
			{
				DeleteDC(dc);
				Device->getLogger()->log("WinText2Image: can't create font");
				return 0;
			}

		SelectObject(dc, font);
		SetTextAlign(dc,TA_LEFT | TA_TOP | TA_NOUPDATECP);

		//
		//Creating image
		//
		video::IImage* theimage = 0;
		{
			//Auto-size needed?
			UINT text_format = 
				(text_multiline ? 0 : DT_SINGLELINE) | (text_no_wordbreak ? 0 : DT_WORDBREAK) |
				(text_left ? DT_LEFT : 0) | (text_center ? DT_CENTER : 0) | (text_right ? DT_RIGHT : 0) |
				(text_top ? DT_TOP : 0) | (text_vcenter ? DT_VCENTER : 0) | (text_bottom ? DT_BOTTOM : 0) |
				DT_NOPREFIX | DT_EDITCONTROL; 

			RECT temp_rect; 
			u32 oldtexWidth = texWidth;
			u32 oldtexHeight = texHeight;
			if (text_autosize_texture_to_fit)
				for (int step = 1 ;true;step++)
				{
//					std::cout << std::endl << " ->> Step - " << step << " !" << std::endl;
//					core::stringc logmsg;

					temp_rect.left	= marginLeft; 
					temp_rect.right	= texWidth  - marginRight; 
					temp_rect.top	= marginTop; 
					temp_rect.bottom = texHeight - marginBottom; 

					s32 h = DrawTextW(dc, Text.c_str(), Text.size(), &temp_rect, text_format | DT_CALCRECT);//if need auto size to fit
					//h new offered height 

					f32 newAspect = f32((f32)(temp_rect.right + marginRight) / (f32)(temp_rect.bottom + marginBottom));
/*
					std::cout << std::endl << " ->> Text Width - " << texWidth << " / "  << texHeight << std::endl;
					logmsg = "  to RECT -> ";
					logmsg += (u32) (temp_rect.right - temp_rect.left);
					logmsg += " / ";
					logmsg += (u32) (temp_rect.bottom - temp_rect.top);
					logmsg += " h (";
					logmsg += (s32) (h);
					logmsg += ") = ";
					logmsg += (u32) ((temp_rect.right - temp_rect.left) * (temp_rect.bottom - temp_rect.top));
					logmsg += " / Aspect = ";
					logmsg += newAspect;
					logmsg += " / ";
					logmsg += texAspect;
					Device->getLogger()->log(logmsg.c_str());

					Device->getLogger()->log("");
/**/
					if (!h || (texWidth > WinText2Image_MAX_TEXTURE_SIZE) || (texHeight > WinText2Image_MAX_TEXTURE_SIZE) ||
						(step >= 400 ) || !text_multiline || !text_autosize_texture_to_fit_advanced) 
						{
							texWidth = temp_rect.right + marginRight;
							if (!ROUNDTO)
								{
									texWidth = getTextureSizeFromSurfaceSize(texWidth);
									texHeight = getTextureSizeFromSurfaceSize(texHeight);
								}
							else
								{
								if (texWidth % ROUNDTO) texWidth += (ROUNDTO - (texWidth % ROUNDTO));
								if (texHeight % ROUNDTO) texHeight += (ROUNDTO - (texHeight % ROUNDTO));
								}
							if (texWidth < oldtexWidth)
								texWidth = oldtexWidth;
							texHeight = temp_rect.bottom + marginBottom;
							if (texHeight < oldtexHeight)
								texHeight = oldtexHeight;
							break;
						}
					if ((h > 0) && (newAspect >= texAspect))
						{
/*							logmsg = "OK new RECT -> ";
							logmsg += (u32) (temp_rect.right - temp_rect.left);
							logmsg += " / ";
							logmsg += (u32) (temp_rect.bottom - temp_rect.top);
							logmsg += " h (";
							logmsg += (s32) (h);
							logmsg += ") = ";
							logmsg += (u32) ((temp_rect.right - temp_rect.left) * (temp_rect.bottom - temp_rect.top));
							logmsg += " / Aspect = ";
							logmsg += newAspect;
							logmsg += " / ";
							logmsg += texAspect;
							Device->getLogger()->log(logmsg.c_str());
/**/
//							texWidth = temp_rect.right + marginRight;
//							texHeight = temp_rect.bottom + marginBottom;
//							texWidth = temp_rect.right + marginRight;
//							texHeight = temp_rect.bottom + marginBottom;
							break;
						}
					else
						{
//							std::cout << "Old Height: " << old_rectHeight << std::endl;
//							std::cout << "New Height: " << new_rectHeight << std::endl;
//							int h_inc = (new_rectHeight - old_rectHeight) / 20;
//							int h_inc = (new_rectHeight - old_rectHeight);
//							int h_inc = u32((f32(h)*(texAspect-newAspect))/10);
//							if (h_inc > 100) h_inc=150;
//							if (h_inc < 1) h_inc=1;
//							if (h_inc < 10) h_inc=10;

							//calculating width increment step
							u32 h_inc = 8;
							h_inc = u32((f32(h)*(texAspect-newAspect))/10);
//							if (h > (s32)texHeight)
//								h_inc = (h-texHeight) / 5;
							if (h_inc > 256) h_inc=256;
							if (h_inc < 8) h_inc=8;
							h_inc += (h_inc % 8);
//							std::cout << "Increment: " << h_inc << std::endl;
							texWidth += h_inc;
							texHeight = u32((f32)texWidth / texAspect);

							if (!ROUNDTO)
								{
									texWidth = getTextureSizeFromSurfaceSize(texWidth);
									texHeight = getTextureSizeFromSurfaceSize(texHeight);
								}
							else
								{
								if (texWidth % ROUNDTO) texWidth += (ROUNDTO - (texWidth % ROUNDTO));
								if (texHeight % ROUNDTO) texHeight += (ROUNDTO - (texHeight % ROUNDTO));
								}
/*
							logmsg = "DrowText Result : move to :: ";
							logmsg += (u32) (texWidth);
							logmsg += " / ";
							logmsg += (u32) (texHeight);
							Device->getLogger()->log(logmsg.c_str());
							Device->getLogger()->log("");
/**/
							continue;
						}

				}

/*
			core::stringc logmsg = "DrowText Result : before :: ";
			logmsg += (u32) (texWidth);
			logmsg += " / ";
			logmsg += (u32) (texHeight);
			Device->getLogger()->log(".... Resizing.....");
			Device->getLogger()->log(logmsg.c_str());
/**/

			if (text_texture_keep_aspect_ratio)
			{
				f32 newtexAspect = ((f32)texWidth / (f32)texHeight);
				if (texAspect > newtexAspect)
					{
						u32 utemp;
						utemp = (u32) ((f32)texHeight * texAspect);
						if (utemp) texWidth = utemp;
					}
				else
				if (texAspect < newtexAspect)
					{
						u32 utemp;
						utemp = (u32) ((f32)texWidth * texAspect);
						if (utemp) texHeight = utemp;
					}
			}
/*

			logmsg = "DrowText Result : after :: ";
			logmsg += (u32) (texWidth);
			logmsg += " / ";
			logmsg += (u32) (texHeight);
			Device->getLogger()->log(logmsg.c_str());
/**/

			if (texWidth  > WinText2Image_MAX_TEXTURE_SIZE) 
				{
					texWidth  = WinText2Image_MAX_TEXTURE_SIZE;
					if (text_texture_keep_aspect_ratio)
						{
							u32 utemp;
							if (texAspect > 1)
								utemp = (u32) ((f32)texWidth / texAspect);
							else
								utemp = (u32) ((f32)texWidth * texAspect);
							if (utemp) texHeight = utemp;
						}
				}
			if (texHeight > WinText2Image_MAX_TEXTURE_SIZE) 
				{
					texHeight = WinText2Image_MAX_TEXTURE_SIZE;
					if (text_texture_keep_aspect_ratio)
						{
							u32 utemp;
							if (texAspect > 1)
								utemp = (u32) ((f32)texHeight / texAspect);
							else
								utemp = (u32) ((f32)texHeight * texAspect);
							if (utemp) texWidth = utemp;
						}
				}
/*

			logmsg = "DrowText Result : RESIZE TO :: ";
			logmsg += (u32) (texWidth);
			logmsg += " / ";
			logmsg += (u32) (texHeight);
			Device->getLogger()->log(logmsg.c_str());
/**/
/*
			if (text_texture_keep_aspect_ratio)
			{
				f32 newtexAspect = ((f32)texWidth / (f32)texHeight);
				if (texAspect < newtexAspect)
					{
						u32 utemp;
						utemp = (u32) ((f32)texHeight / texAspect);
						if (utemp) texWidth = utemp;
					}
				else
				if (texAspect > newtexAspect)
					{
						u32 utemp;
						utemp = (u32) ((f32)texWidth / texAspect);
						if (utemp) texHeight = utemp;
					}
			}

			logmsg = "DrowText Result : after :: ";
			logmsg += (u32) (texWidth);
			logmsg += " / ";
			logmsg += (u32) (texHeight);
			Device->getLogger()->log(logmsg.c_str());
/**/
		if (!ROUNDTO)
			{
				texWidth = getTextureSizeFromSurfaceSize(texWidth);
				texHeight = getTextureSizeFromSurfaceSize(texHeight);
			}
		else
			{
			if (texWidth % ROUNDTO) texWidth += (ROUNDTO - (texWidth % ROUNDTO));
			if (texHeight % ROUNDTO) texHeight += (ROUNDTO - (texHeight % ROUNDTO));
			}

			if (text_texture_keep_aspect_ratio) //test to eleminate any error
				{
				if (texWidth  > WinText2Image_MAX_TEXTURE_SIZE) 
					{
						texWidth  = WinText2Image_MAX_TEXTURE_SIZE;
					}
				if (texHeight > WinText2Image_MAX_TEXTURE_SIZE) 
					{
						texHeight = WinText2Image_MAX_TEXTURE_SIZE;
					}
				}

/*
			logmsg = "DrowText Result : DROW TO :: ";
			logmsg += (u32) (texWidth);
			logmsg += " / ";
			logmsg += (u32) (texHeight);
			Device->getLogger()->log(logmsg.c_str());
/**/
			u32 textureWidth = texWidth;
			u32 textureHeight = texHeight;
			if (!ROUNDTO_TEXTURE)
				{
					textureWidth = getTextureSizeFromSurfaceSize(textureWidth);
					textureHeight = getTextureSizeFromSurfaceSize(textureHeight);
				}
			else
				{
				if (textureWidth % ROUNDTO_TEXTURE) textureWidth += (ROUNDTO_TEXTURE - (textureWidth % ROUNDTO_TEXTURE));
				if (textureHeight % ROUNDTO_TEXTURE) textureHeight += (ROUNDTO_TEXTURE - (textureHeight % ROUNDTO_TEXTURE));
				}
			//
			// make a new bitmap
			//
			HBITMAP bmp = CreateCompatibleBitmap(dc, textureWidth, textureHeight);
			HDC bmpdc = CreateCompatibleDC(dc);

			LOGBRUSH lbrush;
			lbrush.lbColor = TextFormat->Color.Color;
			lbrush.lbHatch = 0;
			lbrush.lbStyle = BS_SOLID;

			HBRUSH brush = CreateBrushIndirect(&lbrush);
			HPEN pen = CreatePen(PS_NULL, 0, 0);

			HGDIOBJ oldbmp		= SelectObject(bmpdc, bmp);
			HGDIOBJ oldbmppen	= SelectObject(bmpdc, pen);
			HGDIOBJ oldbmpbrush = SelectObject(bmpdc, brush);
			HGDIOBJ oldbmpfont	= SelectObject(bmpdc, font);

			SetTextColor(bmpdc, TextFormat->fontColor.Color);

			//
			// draw the letters...
			//
			RECT rect; 
			rect.left	= marginLeft; 
			rect.right	= texWidth  - marginRight; 
			rect.top	= marginTop; 
			rect.bottom = texHeight - marginBottom; 

			// Add ellipsis - "..."
			text_format |=	(text_ellipses_no_textend ? 0: DT_END_ELLIPSIS) | (text_ellipses_no_wordend ? 0 : DT_WORD_ELLIPSIS) | (text_ellipses_filepath ? DT_PATH_ELLIPSIS : 0);
/*
			UINT text_format_ellipsis =	(text_ellipses_no_textend ? 0: DT_END_ELLIPSIS) | (text_ellipses_no_wordend ? 0 : DT_WORD_ELLIPSIS) | (text_ellipses_filepath ? DT_PATH_ELLIPSIS : 0);
			if (text_format_ellipsis && text_autosize_texture_to_fit)
				{
					RECT temp_rect = rect;
					s32 x = DrawTextW(dc, Text.c_str(), Text.size(), &temp_rect, text_format | DT_CALCRECT);//if need auto size to fit
					//x new offered height 
					if (x && (x < rect.bottom - rect.top))
						text_format = text_format | text_format_ellipsis;
				}
			else
				text_format = text_format | text_format_ellipsis;
				/**/

			if (Frame.Active)
				{
				//turn off automatic aliment
//				UINT text_format_temp = text_format ^ (DT_TOP | DT_VCENTER | DT_BOTTOM);
				UINT text_format_temp = text_format & ~(DT_TOP | DT_VCENTER | DT_BOTTOM);
//				UINT text_format = text_format ^ (DT_TOP | DT_VCENTER | DT_BOTTOM);
//				DT_LEFT | DT_CENTER | DT_RIGHT 

				// if - must to make menual vertical alignment
//				if (text_multiline && !text_top)
					{
						RECT temp_rect = rect;
						//int x = DrawTextW(bmpdc, Text.c_str(), Text.size(), &temp_rect, text_format | DT_CALCRECT);//if need auto size to fit
						SetTextColor(bmpdc, TextFormat->Color.Color);// draw invisible text to get it visible height
						int x = DrawTextW(bmpdc, Text.c_str(), Text.size(), &temp_rect, text_format_temp);
//						int x = DrawTextW(bmpdc, Text.c_str(), Text.size(), &temp_rect, text_format);
						SetTextColor(bmpdc, TextFormat->fontColor.Color);
//						if (x) x = temp_rect.bottom - temp_rect.top; 
						if (x && (x < rect.bottom - rect.top))
							if (text_top) //move to top
								{
									x = rect.bottom - rect.top - x;
									rect.bottom -= x;
								}
							else
							if (text_vcenter) //move to vcenter
								{
									x = rect.bottom - rect.top - x;
									rect.bottom -= x / 2; 
									x -= x / 2;
									rect.top += x;
								}
							else //move to bottom
								{
									x = rect.bottom - rect.top - x;
									rect.top += x;
								}
					}
				}
			else
				// if MultiLine - must to make menual vertical alignment
				if (text_multiline && !text_top)
					{
						RECT temp_rect = rect;
						//int x = DrawTextW(bmpdc, Text.c_str(), Text.size(), &temp_rect, text_format | DT_CALCRECT);//if need auto size to fit
						SetTextColor(bmpdc, TextFormat->Color.Color);// draw invisible text to get it visible height
						int x = DrawTextW(bmpdc, Text.c_str(), Text.size(), &temp_rect, text_format);
						SetTextColor(bmpdc, TextFormat->fontColor.Color);
						if (x && (x < rect.bottom - rect.top))
							if (text_vcenter) //move to vcenter
								{
									x = rect.bottom - rect.top - x;
									rect.bottom -= x / 2; 
									x -= x / 2;
									rect.top += x;
								}
							else //move to bottom
								{
									x = rect.bottom - rect.top - x;
									rect.top += x;
								}
					}


//			int x = DrawTextW(bmpdc, Text.c_str(), Text.size(), &rect, text_format);

			//
			// Draw Background
			//
			Rectangle(bmpdc, 0,0,textureWidth+1,textureHeight+1);
			RECT fullrect = {0,0,texWidth,texHeight};
			if (TextFormat->Color.Gradient)
//				GradientFill(bmpdc, fullrect, Frame.Color.Colors);
				GradientFill(bmpdc, fullrect, (COLORREF *) TextFormat->Color.Colors);
//				GradientFillRectVert(bmpdc,&fullrect,TextFormat->Color,TextFormat->GradientToColor); //RGB(255,255,0),RGB(0,255,0));
//				GradientFillRectVert(bmpdc,0,0,texWidth,texHeight,TextFormat->Color,TextFormat->GradientToColor); //RGB(255,255,0),RGB(0,255,0));
//			else
//				Rectangle(bmpdc, 0,0,texWidth+1,texHeight+1);
			SetBkMode(bmpdc, TRANSPARENT);

			//
			// Draw Texture FRAMES
			//
			{
			RECT framesrect;
			framesrect.left		= BasemarginLeft; 
			framesrect.right	= texWidth  - BasemarginRight; 
			framesrect.top		= BasemarginTop; 
			framesrect.bottom	= texHeight - BasemarginBottom; 
			for (int i = 0;i < WT2I_MAX_FRAMES; i++)
				if (Frames.Frame[i].Active) 
					{
						framesrect.left		+= Frames.Frame[i].distanceX + Frames.Frame[i].size; 
						framesrect.right	-= Frames.Frame[i].distanceX + Frames.Frame[i].size; 
						framesrect.top		+= Frames.Frame[i].distanceY + Frames.Frame[i].size; 
						framesrect.bottom	-= Frames.Frame[i].distanceY + Frames.Frame[i].size; 
						DrawFrame(bmpdc,framesrect,Frames.Frame[i]);
					}
			}

			//
			// Draw Text Frame
			//
			DrawFrame(bmpdc,rect,Frame);

//			GradientFill(bmpdc,10,10, L"123456", &Frame.GradientFromColor);
//			GradientFill(bmpdc,10,20, Text.c_str(), &Frame.GradientFromColor);
			if (TextFormat->fontColor.Active)
				if (TextFormat->fontColor.Gradient)
					GradientFill(bmpdc,rect,Text.c_str(), text_format,(COLORREF *) TextFormat->fontColor.Colors);
				else
					DrawTextW(bmpdc, Text.c_str(), Text.size(), &rect, text_format);
			
			/*
			//core::stringc 
			logmsg = "DrowText Result : ";
			logmsg += (s32) (x);
			logmsg += " / ";
			logmsg += (s32) (rect.bottom - rect.top);
			Device->getLogger()->log(logmsg.c_str());
			/**/

			//
			// copy the font bitmap into a new irrlicht image
			//
			BITMAP b;
			PBITMAPINFO pbmi;
			WORD cClrBits;
			u32 cformat;

			// Retrieve the bitmap color format, width, and height.
			GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);

			// Convert the color format to a count of bits.
			cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);

			if (cClrBits <= 8) // we're not supporting these
				cformat = -1;
			else if (cClrBits <= 16)
				cformat = video::ECF_A1R5G5B5;
			else if (cClrBits <= 24)
				cformat = video::ECF_R8G8B8;
			else
				cformat = video::ECF_A8R8G8B8;

			pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
						sizeof(BITMAPINFOHEADER));

			// Initialize the fields in the BITMAPINFO structure.
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = b.bmWidth;
			pbmi->bmiHeader.biHeight = b.bmHeight;
			pbmi->bmiHeader.biPlanes = b.bmPlanes;
			pbmi->bmiHeader.biBitCount = b.bmBitsPixel;

			// If the bitmap is not compressed, set the BI_RGB flag.
			pbmi->bmiHeader.biCompression = BI_RGB;

			// Compute the number of bytes in the array of color
			// indices and store the result in biSizeImage.
			// For Windows NT, the width must be DWORD aligned unless
			// the bitmap is RLE compressed. This example shows this.
			// For Windows 95/98/Me, the width must be WORD aligned unless the
			// bitmap is RLE compressed.
			pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
							* pbmi->bmiHeader.biHeight;
			// Set biClrImportant to 0, indicating that all of the
			// device colors are important.
			pbmi->bmiHeader.biClrImportant = 0;

			LPBYTE lpBits; // memory pointer for bitmap DATA

			PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER) pbmi;
			lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

			GetDIBits(dc, bmp, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);

			//
			// flip bitmap
			//
			s32 rowsize = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8;
			c8 *row = new c8[rowsize];
			
			for (s32 i=0; i < (pbih->biHeight/2); ++i)
			{
				// grab a row
				memcpy(row, lpBits + (rowsize * i), rowsize);
				// swap row
				memcpy(lpBits + (rowsize * i), lpBits + ((pbih->biHeight-1 -i) * rowsize ) , rowsize);
				memcpy(lpBits + ((pbih->biHeight-1 -i) * rowsize ), row , rowsize);
			}

//*

//#define RGB_DISTANCE(R1, G1, B1, R2, G2, B2) ( MAX(MAX(MAX(R1,R2),MAX(G1,G2)),MAX(B1,B2)) )

//#define ABS_DISTANCE(C1, C2) ( (C1 > C2) ? C1 - C2 : C2 - C1  )
//#define RGB_DISTANCE(R1, G1, B1, R2, G2, B2) ( max(max(ABS_DISTANCE(R1,R2),ABS_DISTANCE(G1,G2)),ABS_DISTANCE(B1,B2)) )

			//
			// Set Alpha
			//
			if (cformat == video::ECF_A8R8G8B8)
			{
				u8 r = GetRValue(TextFormat->alpha_Color);
				u8 g = GetGValue(TextFormat->alpha_Color);
				u8 b = GetBValue(TextFormat->alpha_Color);
				DWORD alpha_C = RGB(b,g,r);//exchange b & r 
				// in this case the font should have an alpha channel, but since windows doesn't draw one
				// we have to set one manually by going through all the pixels.. *sigh*

				u8* m;
				DWORD* mm; 
				for (m = lpBits; m < lpBits + pbih->biSizeImage; m+=4)
				{
					mm = (DWORD*)m;
					if (UseAlphaChannel)
					{
//						u8 cDistance = RGB_DISTANCE(m[2],m[1],m[0],0,0,0);
//						if (*mm != RGB(255,0,0)) // pixel has colour
//						if (cDistance > 0) // pixel has colour
//						if (*mm > 0) // pixel has colour
//						if (*mm > 0) // pixel has colour
						if (*mm != alpha_C) // pixel has colour
						{
//							*mm |= (DWORD)0xFF000000; // all pixels are full alpha
//							m[3]=cDistance; // all pixels are full alpha
							m[3]=255; // all pixels are full alpha
						}
					}
					else
						m[3]=255; // all pixels are full alpha
				}

			}
			else if (cformat == video::ECF_A1R5G5B5)
			{
				u8 r = GetRValue(TextFormat->alpha_Color);
				u8 g = GetGValue(TextFormat->alpha_Color);
				u8 b = GetBValue(TextFormat->alpha_Color);
				DWORD alpha_C_32 = RGB(b,g,r);//exchange b & r 
				u16 alpha_C_16 = video::A8R8G8B8toA1R5G5B5(alpha_C_32);

				u8* m;
				for (m = lpBits; m < lpBits + pbih->biSizeImage; m+=2)
				{
					WORD *p = (WORD*)m;
					if (*p != alpha_C_16 || !UseAlphaChannel) // alpha should be set
//					if (m[0] > 0 || !UseAlphaChannel) // alpha should be set
						*p |= 0x8000; // set alpha bit
				}
			}
			else
			{
				cformat = -1;
			}
/**/
			//
			// Make the Irrlicht Image
			//
			if (cformat != -1)
			{
				// turn mip-mapping off
				bool b = Device->getVideoDriver()->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
				theimage = Device->getVideoDriver()->createImageFromData((video::ECOLOR_FORMAT)cformat, core::dimension2d<u32>(textureWidth,textureHeight), (void*)lpBits);
				Device->getVideoDriver()->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS,b);
			}
			else
			{
				Device->getLogger()->log("WinText2Image: Couldn't create font, your pixel format is unsupported.");
			}

			//
			// free memory and windows resources
			//
			delete [] row;
			LocalFree(pbmi);
			GlobalFree(lpBits);
			DeleteDC(bmpdc);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(bmp);

		} // looping through each texture
		DeleteObject(font);
		DeleteDC(dc);
		if (!theimage)
			{
				Device->getLogger()->log("WinText2Image: Something went wrong, aborting.");
				return 0;
			}
		//
		//All is OK! The END!!!
		//
		return theimage;
	}
#endif


