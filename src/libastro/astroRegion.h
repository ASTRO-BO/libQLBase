// ////////////////////////////////////////////////////////////////////////////
//
//  File:      AstroRegion.h
//
//  Version:   1.0
//
//  Author:    Reiner Rohlfs (GADC)
//
//  History:   1.0   13. 12. 03  first released version
//
// ////////////////////////////////////////////////////////////////////////////
#ifndef ROOT_AstroRegion
#define ROOT_AstroRegion

#include <stdio.h>

#include "TLatex.h"
#include "TEllipse.h"
#include "TArrow.h"
#include "TMarker.h"

struct WorldCoor;
class AstroImage;


struct RegionProperties
{
   RegionProperties() {color = 10; arrow[0] = 0; textAngle=0;
                       markerStyle = 24;}
   // default color is white = 10
   RegionProperties( const RegionProperties & prop);

   int      color;
   TString  text;
   char     arrow[3];
   double   textAngle;
   Style_t  markerStyle;
};

//_____________________________________________________________________________

class AstroRegion
{
protected:
   Double_t   fImgX;          // X - position of center in image pixels
   Double_t   fImgY;          // Y - position of center in image pixels
   TString    fRegionText;    // text of a region

public:
   AstroRegion() {}
   AstroRegion(Double_t imgX, Double_t imgY, const char * text);

   Double_t    GetImgX()      {return fImgX;}
   Double_t    GetImgY()      {return fImgY;}

   virtual const char * GetRegionText()                   {return fRegionText.Data();}
   virtual void         SetRegionText(const char * text)  {fRegionText = text;} // *MENU*

   AstroImage *  ExecuteEvent(Int_t event, Double_t fX, Double_t fY);

   virtual  void WriteRegion(FILE * file, WorldCoor * wcs);

ClassDef(AstroRegion, 1)  // Basic region class for an AstroImage
};


//_____________________________________________________________________________

class AstroText : public TLatex, public AstroRegion
{
protected:
public:
   AstroText() {}
   AstroText(Double_t imgX, Double_t imgY, const char * text);
   AstroText(WorldCoor * wcs, char * currentSystem, 
             RegionProperties & globalProperties);

   const char * GetRegionText()                   {return GetTitle();}
   void         SetRegionText(const char * text)  {SetTitle(text);} // *MENU*

   void     WriteRegion(FILE * file, WorldCoor * wcs);

   void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void     Paint(Option_t * option);

ClassDef(AstroText, 1)  // Text for the AstroImage display
};

//_____________________________________________________________________________

class AstroEllipse : public TEllipse, public AstroRegion, public TAttText
{
protected:
   Double_t fImgR1; // radius 1 in image pixel
   Double_t fImgR2; // radius 2 in image pixel

public:
   AstroEllipse() {}
   AstroEllipse(Double_t imgX, Double_t imgY, const char * text);
   AstroEllipse(WorldCoor * wcs, char * currentSystem, 
                RegionProperties & globalProperties);

   void     WriteRegion(FILE * file, WorldCoor * wcs);

   void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void     Paint(Option_t * option);

   Double_t GetImgR1()            {return fImgR1;}
   Double_t GetImgR2()            {return fImgR2;}


   // we don't want to see these functions in the menu
   void   SetX1(Double_t x1) {TEllipse::SetX1(x1);} 
   void   SetY1(Double_t y1) {TEllipse::SetY1(y1);} 
   void   SetTextAttributes() {}

ClassDef(AstroEllipse, 1)  // Ellipse region for the AstroImage display
};

//_____________________________________________________________________________

class AstroCircle : public TEllipse, public AstroRegion, public TAttText
{
protected:
   Double_t fImgR; // radius in image pixel

public:
   AstroCircle() {}
   AstroCircle(Double_t imgX, Double_t imgY, const char * text);
   AstroCircle(WorldCoor * wcs, char * currentSystem, 
               RegionProperties & globalProperties);

   void     WriteRegion(FILE * file, WorldCoor * wcs);

   void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void     Paint(Option_t * option);

   void     SetImgR(Double_t r)  {fImgR = r;}  // *MENU*
   Double_t GetImgR()            {return fImgR;}

   // we don't want to see these functions in the menu
   void   SetR1(Double_t r1) {TEllipse::SetR1(r1);} 
   void   SetR2(Double_t r2) {TEllipse::SetR2(r2);} 
   void   SetTheta(Double_t theta=0) {TEllipse::SetTheta(theta);} 
   void   SetX1(Double_t x1) {TEllipse::SetX1(x1);} 
   void   SetY1(Double_t y1) {TEllipse::SetY1(y1);} 
   void   SetTextAttributes() {}



ClassDef(AstroCircle, 1)  // Circle region for the AstroImage display
};

//_____________________________________________________________________________

class AstroLine : public TArrow, public AstroRegion, public TAttText
{
protected:
   Double_t   fImgX2;    // X - position of second end point of line
   Double_t   fImgY2;    // Y - position of second end point of line

public:
   AstroLine() {}
   AstroLine(Double_t imgX, Double_t imgY, const char * text);
   AstroLine(WorldCoor * wcs, char * currentSystem, 
             RegionProperties & globalProperties);

   Double_t GetImgX2()      {return fImgX2;}
   Double_t GetImgY2()      {return fImgY2;}
   void     WriteRegion(FILE * file, WorldCoor * wcs);

   void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void     Paint(Option_t * option);

   // we don't want to see these functions in the menu
   void   SetTextAttributes() {}
   void   SetTextAngle(Float_t tangle=0) {}


ClassDef(AstroLine, 1)  // Line region for the AstroImage display
};

//_____________________________________________________________________________

class AstroPoint : public TMarker, public AstroRegion, public TAttText
{
protected:

public:
   AstroPoint() {}
   AstroPoint(Double_t imgX, Double_t imgY, const char * text);
   AstroPoint(WorldCoor * wcs, char * currentSystem, 
              RegionProperties & globalProperties);

   void     WriteRegion(FILE * file, WorldCoor * wcs);

   void     ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void     Paint(Option_t * option);

   // we don't want to see these functions in the menu
   void   SetTextAttributes() {}
   void   SetTextAngle(Float_t tangle=0) {}
   void   SetX(Double_t x) {TMarker::SetX(x);} 
   void   SetY(Double_t y) {TMarker::SetY(y);} 

ClassDef(AstroPoint, 1)  // Point region for the AstroImage display
};



#endif

