// This file in a part of LibAn8, Copyright (c) 2002-2007, Gr�gory Smialek
// It's licence can be found in an8loader.h included in this package
// Web Site: texel3d.free.fr
#include "An8Loader.h"
using namespace std;

An8Film::An8Film(void)
{
}

An8Film::~An8Film(void)
{

}

bool An8Film::Load(AN8XPARSER*p_parser)
{
	int elementStart = p_parser->IndexOf(1,"film");
	if(elementStart<0) return false;

	int elementEnd = p_parser->IndexOf(1,"}");
	while(p_parser->Length()!=1+LINERETURN && elementStart != elementEnd)
	{
		if(p_parser->FindWord("size {"))
		{
			p_parser->CutLine("{",2);
			string element = p_parser->SubStr(""," ");
			Size.Width = atoi( element.c_str());

			p_parser->CutLine(" ",1);
			element = p_parser->SubStr(""," ");
			Size.Height = atoi( element.c_str());
		}
		if(p_parser->FindWord("ratio {"))
		{
			p_parser->CutLine("{",2);
			string element = p_parser->SubStr(""," ");
			Ratio.Width = (float)atof( element.c_str());

			p_parser->CutLine(" ",1);
			element = p_parser->SubStr(""," ");
			Ratio.Height = (float)atof( element.c_str());
		}
		p_parser->ReadLine();
		elementEnd =p_parser->IndexOf(1,"}");
	}
	return true;
}
