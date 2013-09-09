// ////////////////////////////////////////////////////////////////////////////
//
//  File:      AstroImage.h
//
//  Version:   1.0
//
//  Author:    Reiner Rohlfs (GADC)
//
//  History:   1.0   13. 12. 03  first released version
//
// ////////////////////////////////////////////////////////////////////////////
#ifndef ROOT_AstroImage
#define ROOT_AstroImage

#include "TAttText.h"

#include "TASImage.h"
#include "TFImage.h"
#include <fitsio.h>

struct WorldCoor;
struct RegionProperties;


//_____________________________________________________________________________

class AstroImage : public TASImage, public TAttLine, public TAttText
{
protected:
   WorldCoor   * fWcs;           //! structure for the wcs funcitons
   Bool_t      fDeg;             // draw coordinate system in units of degree
   Bool_t      fCoordinates;     // draw coordinate system if kTRUE
   Int_t       fAxisAChange;     // increase or decrease number of drawn lines of first Axis
   Int_t       fAxisBChange;     // increase or decrease number of drawn lines of second Axis
   Bool_t      fAxisASwap;       // draw Label of first Axis at the other side
   Bool_t      fAxisBSwap;       // draw Label of second Axis at the other side

public:
// constructors and destructor:

   AstroImage();                        // default constructor

   AstroImage(const char * filename, const char * name, UInt_t cycle);     
                                        // create an image from a file,
                                        // a FITS file, a ROOT file or an ASRO file

   AstroImage(TFBaseImage * image);     // create an image from the image class
                                        // of the AstroROOT - containers.

   AstroImage(fitsfile * fptr);         // create an image from a FITS image

//   AstroImage(const AstroImage & astroImage);
                                        // standard copy constructor

   ~AstroImage();                       // destructor

   Bool_t   ScrPixel2ImgPixel(Int_t scrX, Int_t scrY, Double_t & imgX, Double_t & imgY);
   Double_t GetPixelSizeX();   // Width of one image pixel in frame coordinates [0..1]
   Double_t GetPixelSizeY();   // Height of one image pixel in frame coordinates [0..1]
   Bool_t   ImgPixel2FrameCoord(Double_t imgX, Double_t imgY, Double_t * frX, Double_t * frY);


// functions accessible with the context menu 
// of the right mouse button:

   void   SetTitle(const char * text);  // *MENU*
                                        // A new title in a title box is created or
                                        // an already existing title is updated.    

   void   AddText(const char * text);   // *MENU*
                                        // Once the text is displayed it can be modified
                                        // with the mouse (position, size, color)

   void   AddRegion(const char * type, const char * text); // *MENU*
                                        // Once the region is displayed it can be 
                                        // modified with the mouse (position, size, color)

   void   WriteRegion();                // *MENU*
   void   ReadRegion();                 // *MENU*


   void   CoordinateSystem(const char * coordSystem); // *MENU*
                                        // Defines the displayed coordinates. 

   Bool_t GetDeg()                      {return fDeg;}
   void   SetDeg(Bool_t deg)            {fDeg = deg;}             // *TOGGLE*

   Bool_t GetCoordinates()              {return fCoordinates;}
   void   SetCoordinates(Bool_t coords) {fCoordinates = coords;}  // *TOGGLE*


   void   IncFirstAxis()         {fAxisAChange--;}                   // *MENU*
   void   DecFirstAxis()         {fAxisAChange++;}                   // *MENU*
   void   IncSecondAxis()        {fAxisBChange--;}                   // *MENU*
   void   DecSecondAxis()        {fAxisBChange++;}                   // *MENU*
   void   ResetAxis()            {fAxisAChange = fAxisBChange = 0;}  // *MENU*
   void   SwapFirstAxisLable()   {fAxisASwap = !fAxisASwap;}         // *MENU*
   void   SwapSecondAxisLable()  {fAxisBSwap = !fAxisBSwap;}         // *MENU*

        
// required functions for the ROOT environment:

   void   Draw(Option_t *option = "");

   void   Paint(Option_t *option = ""); // Draws the coordinate system, the text,
                                        // the markers and the header on top of
                                        // the image. This function also prints
                                        // all items into a postscript file.

   char * GetObjectInfo(Int_t px, Int_t py) const;
                                        // Prepares a text with pixel value and 
                                        // coordinates at mouse position. This text    
                                        // will automatically displayed by ROOT
   
   void   SetTextAngle(Float_t tangle=0) {} 
                                        // this is done automatically. The user cannot set

   void   Pop() {}                      // never pop up the image above the other
                                        // items like text and markers


// we don't want to see these functions in the menu
   void  Flip(Int_t flip = 180)        {}      
   void  Mirror(Bool_t vert = kTRUE)   {}
   void  SetImageCompression(UInt_t lcompression)
            { TAttImage::SetImageCompression(lcompression);}
   void  SetImageQuality(EImageQuality lquality)
            { TAttImage::SetImageQuality(lquality);}
    

protected:
   void InitImage(TFBaseImage * image);
   void PaintCoordinates();
   void PaintCoordLine(double axisA1, double axisA2, double axisB1, double axisB2, int scaleType);
   void Read1Region(char * line, char * currentSystem, RegionProperties & globalProperties);

   ClassDef(AstroImage, 1)   // display of an astronomical images


};


#endif



