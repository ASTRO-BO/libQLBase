// ////////////////////////////////////////////////////////////////////////////
//
//  File:      astroImage.cxx
//
//  Version:   1.0.1
//
//  Author:    Reiner Rohlfs (GADC)
//
//  History:   1.0   26.04.04  first released version
//             1.0.1 06.02.05  adopt for ROOT 4.02.00
//
// ////////////////////////////////////////////////////////////////////////////
# include <float.h>
# include <math.h>
# include <string>
# include <stdio.h>

# include <TROOT.h>
# include <TPad.h>
# include <TVirtualPS.h>
# include <TPaveText.h>
# include <TMarker.h>
# include <TPoint.h>
# include <TMath.h>
# include <TGFileDialog.h>

#include "astroImage.h"
#include "astroRegion.h"
#include "wcs.h"


ClassImp(AstroImage)

//_____________________________________________________________________________
// AstroImage can display astronomical images with their coordinate system.
// Call the Draw() - function to draw the image on a TPad.
//
// The libwcs library is used for coordinate transformations. 
// 
// The color palette editor of TASImage (see TASImage::StartPaletteEditor() and
// TPaletteEditor ) is used to manipulate the palette.
// 
// Regions can be defined or read from a region file. 

static double deg_min_sec[16] = {
45,              //  45 deg
20,              //  20 deg
10,              //  10 deg
5,               //   5 deg
2,               //   2 deg
1,               //   1 deg
0.5,             //  30'
1./6. ,          //  10'
1./12,           //   5'
1./30,           //   2'
1./60,           //   1'
1./120,          //  30"
1./360,          //  10"
1./720,          //   5"
1./1800,         //   2"
1./3600          //   1"
};

static double hh_mm_ss[15] =  { // for ra / dec
45,              //   3 h
15,              //   1 h
7.5,             //  30 min
2.5,             //  10 min
1.25,            //   5 min
0.5,             //   2 min
0.25,            //   1 min
0.125,           //  30 sec
0.125/3,         //  10 sec
0.125/6,         //   5 sec
0.125/15,        //   2 sec
0.125/30,        //   1 sec
0.0125/6,        //   0.5 sec
0.0125/15,       //   0.2 sec
0.0125/30        //   0.1 sec
};


//_____________________________________________________________________________
static inline bool CoordsOut(double x, double y, double right_gap, double bottom_gap)
{
   return x < 0 || x > right_gap || y > 1 || y < bottom_gap;
}
//_____________________________________________________________________________
static void BorderCoords(double inX, double inY, double & outX, double & outY,
                         double right_gap, double bottom_gap, int iter = 8)
{
   if (iter <= 0) return;
   iter--;

   double cx = (inX + outX) / 2;
   double cy = (inY + outY) / 2;

   if (CoordsOut(cx, cy, right_gap, bottom_gap))
      // middle point is out
      {
      outX = cx;
      outY = cy;
      BorderCoords(inX, inY, outX, outY, right_gap, bottom_gap, iter);
      }
   else
      // middle point is in
      BorderCoords(cx, cy, outX, outY, right_gap, bottom_gap, iter);

}
//_____________________________________________________________________________
static void GetLinePos(double min, double coordDiff, UInt_t pixelDiff, 
                       double * first, double * offset, int userChange,
                       int scaleType)
{

   // first determine offset
   // at least 100 pixel between two lines
   int type = 1;
   UInt_t numDiff = pixelDiff / 100 + 1;
   *offset = coordDiff / numDiff;

   if (scaleType == 0)
      // deg.fraction
      {
      // make a nice offset 2, 5 or 10
      double logOffset = TMath::Log10(*offset);
      *offset = pow(double(10), (int)logOffset);
      if (logOffset - (int)logOffset > 0.7)
         {*offset *= 10;  type = 1;}
      else if (logOffset - (int)logOffset > 0.3)
         {*offset *= 5;   type = 5;}
      else if (logOffset - (int)logOffset > 0)
         {*offset *= 2;   type = 2;}
      else if (logOffset - (int)logOffset < -0.3)
         {*offset *= .5;  type = 5;}
      else if (logOffset - (int)logOffset <  -0.7)
         {*offset *= 0.2; type = 2;}

      while (userChange < 0)
         {
         switch (type)
            {
            case 1:  *offset /= 2;    type = 5;   break;
            case 5:  *offset /= 2.5;  type = 2;   break;
            case 2:  *offset /= 2;    type = 1;   break;
            }
         userChange++;
         }

      while (userChange > 0)
         {
         switch (type)
            {
            case 1:  *offset *= 2;    type = 2;   break;
            case 2:  *offset *= 2.5;  type = 5;   break;
            case 5:  *offset *= 2;    type = 1;   break;
            }
         userChange--;
         }

      if (*offset > 45) *offset = 45;
      }

   else if (scaleType == 1)
      {
      // deg.min.sec
      int index = 1;
      while (index < 16 && *offset < deg_min_sec[index])
         index++;
      index -= userChange;
      if (index < 1) index = 1;
      if (index > 16) index = 16;
      *offset = deg_min_sec[index-1];
      }
   else if (scaleType == 2)
      {
      // hh:mm:ss.fr
      int index = 1;
      while (index < 15 && *offset < hh_mm_ss[index])
         index++;
      index -= userChange;
      if (index < 1) index = 1;
      if (index > 15) index = 15;
      *offset = hh_mm_ss[index-1];
      }

   // get position of first line
   *first = (int)(min / *offset) * *offset;
   if (min > 0) 
      *first += *offset;

}

//_____________________________________________________________________________

static const char * GetLabel(double value, int scaleType)
{
   static char label[20];

   if (scaleType == 0)    // deg.fraction
      sprintf(label, "  %-7.3f", value);

   else if (scaleType == 1)  // dd:min:sec
      {
      if (value < 0.0)
         sprintf(label, "  %3d:%02d:%02d", (int)(value - 0.00014),
                          -(int)((value * 60) - 0.0083) % 60, 
                          -(int)((value * 3600) - 0.5)  % 60 );
      else
         sprintf(label, "  %3d:%02d:%02d", (int)(value + 0.00014),
                          (int)((value * 60) + 0.0083) % 60, 
                          (int)((value * 3600) + 0.5)  % 60 );
      }
   else
      {
      if (value < 0.0)
         sprintf(label, "  %2d:%02d:%04.1f", (int)(value / 15 - 0.000014),
                          -(int)((value * 4) - 0.00083) % 60, 
                          -(int)((value * 2400) - 0.5)  % 600 / 10.0 );
      else
         sprintf(label, "  %2d:%02d:%04.1f", (int)(value / 15 + 0.000014),
                          (int)((value * 4) + 0.00083) % 60, 
                          (int)((value * 2400) + 0.5) % 600  / 10.0);
      }

   return label;
}

//_____________________________________________________________________________

template <class T, class F> 
void CopyImage(TFImage<T, F> * image, UInt_t width, UInt_t height, 
               double * imgArray)
{
   T nullVal = image->GetNull();
   Bool_t nullDef = image->NullDefined();

   if (image->IsSubSection())
      {
      for (UInt_t x = 0; x < width; x++)
         for (UInt_t y = 0; y < height; y++)
            {
            T val = (*image)(y)(x);
            if (nullDef && nullVal == val)
               imgArray[x + y * width] = 0;
            else
               imgArray[x + y * width] = val;
            }
      }
   else
      {
      for (UInt_t x = 0; x < width; x++)
         for (UInt_t y = 0; y < height; y++)
            {
            T val = (*image)[y][x];
            if (nullDef && nullVal == val)
               imgArray[x + y * width] = 0;
            else
               imgArray[x + y * width] = val;
            }
      }
}

//_____________________________________________________________________________
AstroImage::AstroImage()
{
// default constructor

   fWcs = NULL;
   SetLineColor(10);
   SetTextColor(10);
   SetTextSize(0.03);
   fDeg         = kTRUE;
   fCoordinates = kTRUE;
   fAxisAChange = fAxisBChange = 0;
   fAxisASwap   = fAxisBSwap   = kFALSE;
}
//_____________________________________________________________________________
AstroImage::AstroImage(const char * filename, const char * name, UInt_t cycle)
   : TASImage()
{
// Reads an image from a file. This can be either a FITS file or an ROOT / ASRO 
// file with a TFBaseImage.
// 
// Parameter filename is the FITS, ROOT or ASRO file name. The extension  
// defines the file format.
//
// Parameter name is the name of the image or the extension name in 
// FITS files. It can be an empty string for FITS files, but must be 
// defined for ROOT and ASRO files.
//
// Parameter cycle defines the extension in FITS files or the circle number
// in ROOT and ASRO files. The  primary array in a FITS file has extension 
// number 1 and the first extension is  number  2.  In  ROOT  and  in 
// ASRO  files each image name has its own cycle counter.  Therefore  
// several  images can have the same cycle number in a ROOT or ASRO  
// file  if  they  have  different names. If it is set to 0 the first
// image in the file with the defined image name is displayed.

   fWcs = NULL;
   SetLineColor(10);
   SetTextColor(10);
   SetTextSize(0.03);
   fDeg         = kTRUE;
   fCoordinates = kTRUE;
   fAxisAChange = fAxisBChange = 0;
   fAxisASwap   = fAxisBSwap   = kFALSE;

   TFBaseImage * image = TFReadImage(filename, name, cycle);
   if (image)
      InitImage(image);
   delete image;
}
//_____________________________________________________________________________
AstroImage::AstroImage(TFBaseImage * image)
{
// Displays the image 
// All data are copied from the image. The image is not used any more after 
// the construction of AstroImage and can be deleted.

   fWcs = NULL;
   SetLineColor(10);
   SetTextColor(10);
   SetTextSize(0.03);
   fDeg         = kTRUE;
   fCoordinates = kTRUE;
   fAxisAChange = fAxisBChange = 0;
   fAxisASwap   = fAxisBSwap   = kFALSE;

   if (image)
      InitImage(image);
}
//_____________________________________________________________________________
AstroImage::AstroImage(fitsfile * fptr)
{
// Displays a FITS image

   fWcs = NULL;
   SetLineColor(10);
   SetTextColor(10);
   SetTextSize(0.03);
   fDeg         = kTRUE;
   fCoordinates = kTRUE;
   fAxisAChange = fAxisBChange = 0;
   fAxisASwap   = fAxisBSwap   = kFALSE;

   int status = 0;
   int numAxis;

   fits_get_img_dim(fptr, &numAxis, &status);
   if (numAxis != 2)    return;

   long size[2];
   fits_get_img_size(fptr, 2, size, &status);
   if (status != 0)     return;

   double * imgArray = new double[size[0] * size[1]];
   double nullVal = DBL_MAX;
   int    anyNull = 0;
   long   start[2] = {1, 1};

   char * header = NULL;
   fits_get_image_wcs_keys(fptr, &header, &status);

   fits_read_pix(fptr, TDOUBLE, start, size[0] * size[1], 
                 &nullVal, imgArray, &anyNull, &status);
   
   if (status == 0)
      {
      if (anyNull)
         for (long pix = 0; pix < size[0] * size[1]; pix++)
            if (imgArray[pix] == DBL_MAX)
               imgArray[pix] = 0;

      fWcs = wcsinit(header);
      SetImage(imgArray, size[0], size[1], NULL);
      }

   free(header);
   delete [] imgArray;
}
//_____________________________________________________________________________
AstroImage::~AstroImage()
{
   if (fWcs)
      wcsfree(fWcs);
}
//_____________________________________________________________________________
void AstroImage::InitImage(TFBaseImage * image)
{
// Protected function to initialize this class with the input image

   UInt_t width, height;

   UInt_t numDim = image->GetNumDim(image->IsSubSection());
   if (numDim < 2)   return;

   UInt_t * size = new UInt_t[numDim];
   image->GetSize(size, image->IsSubSection());
   height = size[0];
   width  = size[1];

   // create a header string like cfitsio does it and fill the 
   // WorldCoor structure
   TFAttrIter i_attr = image->MakeAttrIterator();
   char hstr[200];
   char hstr2[100];
   std::string header;

   sprintf(hstr,"NAXIS   = %3d%67s", numDim, " ");
   header += hstr;
   for (int dim = 0; dim < numDim; dim++)
      {
      sprintf(hstr,"NAXIS%d  = %5d%65s", dim+1, size[numDim - 1 - dim], " ");
      header += hstr;
      }
   delete [] size;

   while (i_attr.Next())
      {
      if (i_attr->IsA() == TFStringAttr::Class())
         sprintf(hstr, "%-8s= '%-8s'", i_attr->GetName(), i_attr->GetStringValue(hstr2));
      else if (i_attr->IsA() == TFDoubleAttr::Class())
         sprintf(hstr, "%-8s= %20s", i_attr->GetName(), i_attr->GetStringValue(hstr2, 0, "%20.15f"));
      else
         sprintf(hstr, "%-8s= %20s", i_attr->GetName(), i_attr->GetStringValue(hstr2));
      strncat(hstr, 
      "                                                                        ",
      80 - strlen(hstr));
      hstr[80] = 0;
      header += hstr;
      }
   fWcs = wcsinit(header.c_str());

   double * imgArray = new double[width * height];

   if (image->IsA() == TFBoolImg::Class())
      CopyImage( (TFBoolImg*)image, width, height, imgArray);

   else if (image->IsA() == TFCharImg::Class())
      CopyImage( (TFCharImg*)image, width, height, imgArray);

   else if (image->IsA() == TFUCharImg::Class())
      CopyImage( (TFUCharImg*)image, width, height, imgArray);

   else if (image->IsA() == TFShortImg::Class())
      CopyImage( (TFShortImg*)image, width, height, imgArray);

   else if (image->IsA() == TFUShortImg::Class())
      CopyImage( (TFUShortImg*)image, width, height, imgArray);

   else if (image->IsA() == TFIntImg::Class())
      CopyImage( (TFIntImg*)image, width, height, imgArray);

   else if (image->IsA() == TFUIntImg::Class())
      CopyImage( (TFUIntImg*)image, width, height, imgArray);

   else if (image->IsA() == TFFloatImg::Class())
      CopyImage( (TFFloatImg*)image, width, height, imgArray);

   else if (image->IsA() == TFDoubleImg::Class())
      CopyImage( (TFDoubleImg*)image, width, height, imgArray);
   else
      {
      delete [] imgArray;
      return;
      }


   SetImage(imgArray, width, height, NULL);

   SetName(image->GetName());

   delete [] imgArray;
}
//_____________________________________________________________________________
char * AstroImage::GetObjectInfo(Int_t px, Int_t py) const
{
// Returns the information of the pixel below the mouse to be displayed 
// in the status bar of the TCanvas.

   char * TASImageText = TASImage::GetObjectInfo(px, py);
   if (!iswcs(fWcs)) return TASImageText;

   int         imgX, imgY;
   if (sscanf(TASImageText, "x: %d  y: %d", &imgX, &imgY) != 2)
      return TASImageText;

   static char info [128];
   double axisA, axisB;
   pix2wcs(fWcs, imgX, imgY, &axisA, &axisB);
   sprintf(info, "%s: %7.3f / %7.3f  %s",
          getwcsin(fWcs), axisA, axisB, TASImageText);

   return info;

}
//_____________________________________________________________________________
void AstroImage::Draw(Option_t *option)
{
// Draws (registers) this image on the current TPad

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
   if (!gPad)
      gROOT->MakeDefCanvas();
#else
   if (!gPad)
      (gROOT->GetMakeDefCanvas())();
#endif

   if ( fabs(gPad->GetRightMargin()  - 0.1) < 0.001 &&
        fabs(gPad->GetLeftMargin()   - 0.1) < 0.001 &&
        fabs(gPad->GetTopMargin()    - 0.1) < 0.001 &&
        fabs(gPad->GetBottomMargin() - 0.1) < 0.001 ) 
      {
      gPad->SetTopMargin(0.03);
      gPad->SetBottomMargin(0.03);
      gPad->SetRightMargin(0.15);
      gPad->SetLeftMargin(0.03);
      }

   TASImage::Draw(option);
}
//_____________________________________________________________________________
void AstroImage::Paint(Option_t *option)
{
// Draws this image with the coordinate system on the current TPad

   TASImage::Paint(option);

   // draw the coordinate system
   if (fCoordinates)
      PaintCoordinates();
}
//_____________________________________________________________________________
void AstroImage::PaintCoordinates()
{
// Protected function to draw the astronomical coordinates.

   if (!iswcs(fWcs)) return;

   TAttLine::Modify(); 
   TAttText::Modify(); 


   // variables to convert image pixel to screen coordinate
   Double_t xconv = gPad->AbsPixeltoX(GetScaledWidth()  + gPad->XtoAbsPixel(0)) / fZoomWidth;
   Double_t yconv = -1.0 * gPad->AbsPixeltoY(GetScaledHeight() + gPad->YtoAbsPixel(0)) / fZoomHeight;
   Double_t bottom_gap = 1 + gPad->AbsPixeltoY(GetScaledHeight() + gPad->YtoAbsPixel(0));
   Double_t right_gap  = gPad->AbsPixeltoX(GetScaledWidth()  + gPad->XtoAbsPixel(0));

   // get ranges for axisA and axisB of the actually drawn image
   double limAxisA[5], minA, maxA;
   double limAxisB[5], minB, maxB;

   pix2wcs(fWcs, fZoomOffX,              fZoomOffY,               limAxisA + 0, limAxisB + 0);
   pix2wcs(fWcs, fZoomOffX + fZoomWidth, fZoomOffY,               limAxisA + 1, limAxisB + 1);
   pix2wcs(fWcs, fZoomOffX + fZoomWidth, fZoomOffY + fZoomHeight, limAxisA + 2, limAxisB + 2);
   pix2wcs(fWcs, fZoomOffX,              fZoomOffY + fZoomHeight, limAxisA + 3, limAxisB + 3);
   pix2wcs(fWcs, fZoomOffX + fZoomWidth / 2, fZoomOffY + fZoomHeight / 2, limAxisA + 4, limAxisB + 4);


   minB = maxB = limAxisB[0];
   for (int corner = 1; corner < 4; corner++)
      {
      if (minB > limAxisB[corner]) minB = limAxisB[corner];
      if (maxB < limAxisB[corner]) maxB = limAxisB[corner];
      }

   bool   polOnScreen = false;
   int    offImage;
   double imgX, imgY;
   wcs2pix(fWcs, 180, 90, &imgX, &imgY, &offImage);
   if (!offImage) 
      {
      double screenX = (imgX - fZoomOffX) * xconv;
      double screenY = (imgY - fZoomOffY) * yconv + bottom_gap;
      if (!CoordsOut(screenX, screenY, right_gap, bottom_gap))
         {
         maxB = 89.999;   // not 90.0 because we dont't want to see the
                          // 90 deg label in the center
         minA = 0;
         maxA = 360;
         polOnScreen = true;
         }
      }
      
   wcs2pix(fWcs, 180, -90, &imgX, &imgY, &offImage);
   if (!offImage) 
      {
      double screenX = (imgX - fZoomOffX) * xconv;
      double screenY = (imgY - fZoomOffY) * yconv + bottom_gap;
      if (!CoordsOut(screenX, screenY, right_gap, bottom_gap))
         {
         minB = -89.999;
         minA = 0;
         maxA = 360;
         polOnScreen = true;
         }
      }
      

   if (!polOnScreen)
      {
      double diff1 = limAxisA[0] - limAxisA[4];
      double diff2 = limAxisA[4] - limAxisA[2];
      double diff3 = limAxisA[1] - limAxisA[4];
      double diff4 = limAxisA[4] - limAxisA[3];

      bool wrapAround = (diff1 * diff2 < 0.0) || (diff3 * diff4 < 0);
      if (wrapAround)
         {
         // we have a wrap around in the A-axis
         minA = maxA = limAxisA[0];
         for (int corner = 1; corner < 4; corner++)
            {
            if (minA > limAxisA[corner]) minA = limAxisA[corner];
            if (maxA < limAxisA[corner]) maxA = limAxisA[corner];
            }
         for (int corner = 0; corner < 4; corner++)
            if ( fabs(limAxisA[corner] - minA) < fabs(limAxisA[corner] - maxA))
               limAxisA[corner] += 360;
         }


      minA = maxA = limAxisA[0];
      for (int corner = 1; corner < 4; corner++)
         {
         if (minA > limAxisA[corner]) minA = limAxisA[corner];
         if (maxA < limAxisA[corner]) maxA = limAxisA[corner];
         }

      // sometimes the maximum/minimum is not at the corner. We increase it a little bit
      minA -= (maxA - minA) * 0.1;
      maxA += (maxA - minA) * 0.1;
      minB -= (maxB - minB) * 0.1;
      maxB += (maxB - minB) * 0.1;
      }
   
   // draw axis A
   double axisOff;
   double axisPos;
   int    scaleType;   // 0: deg.fraction   1: deg:min:sec   2: hh:min:sec
   UInt_t imageLenght = (UInt_t)sqrt(double(GetScaledWidth() * GetScaledWidth() +
                                     GetScaledHeight() * GetScaledHeight()) );  

   if (fDeg)   scaleType = 0;
   else 
      {
      char * currSys = getwcsin(fWcs);
      if (strcmp(currSys, "LINEAR")   == 0 ||
          strcmp(currSys, "PHYSICAL") == 0    )
         {
         char * oriSys = getradecsys(fWcs);
         if (strcmp(oriSys, "GALACTIC") == 0 ||
             strcmp(oriSys, "ECLIPTIC") == 0    )
            scaleType = 1;
         else
            scaleType = 2;
         }
      else if (strcmp(currSys, "GALACTIC") == 0 ||
               strcmp(currSys, "ECLIPTIC") == 0    )
         scaleType = 1;
      else 
         scaleType = 2;
      }

   GetLinePos(minA, maxA - minA, imageLenght, &axisPos, &axisOff, fAxisAChange,
              scaleType);
   while (axisPos < maxA)
      {
      if (fAxisASwap)
         PaintCoordLine(axisPos, axisPos, maxB, minB, scaleType);
      else
         PaintCoordLine(axisPos, axisPos, minB, maxB, scaleType);
      axisPos += axisOff;
      }

   // draw axis B
   scaleType = fDeg ? 0 : 1;
   GetLinePos(minB, maxB - minB, imageLenght, &axisPos, &axisOff, fAxisBChange,
              scaleType);
   while (axisPos < maxB)
      {
      if (fAxisBSwap)
         PaintCoordLine(minA, maxA, axisPos, axisPos, scaleType);
      else
         PaintCoordLine(maxA, minA, axisPos, axisPos, scaleType);
      axisPos += axisOff;
      }

}
//_____________________________________________________________________________
void AstroImage::PaintCoordLine(double axisA1, double axisA2, 
                                double axisB1, double axisB2,
                                int scaleType)
{
// Protected function to draw one line of the coordinate system.

   Int_t    numPt = 0;
   double   screenX[100], screenY[100];
   double   prevScreenX, prevScreenY;
   bool     labelDrawn = false;
   double   deltaAxisA = (axisA2 - axisA1) / 99;
   double   deltaAxisB = (axisB2 - axisB1) / 99;
   int      status  = 0;  // 0: out not jet in,  1: in for first time,  
                          // 2: in 3: out for the first time

   // variables to convert image pixel to screen coordinate
   Double_t xconv = gPad->AbsPixeltoX(GetScaledWidth()  + gPad->XtoAbsPixel(0)) / fZoomWidth;
   Double_t yconv = -1.0 * gPad->AbsPixeltoY(GetScaledHeight() + gPad->YtoAbsPixel(0)) / fZoomHeight;
   Double_t bottom_gap = 1 + gPad->AbsPixeltoY(GetScaledHeight() + gPad->YtoAbsPixel(0));
   Double_t right_gap  = gPad->AbsPixeltoX(GetScaledWidth()  + gPad->XtoAbsPixel(0));

   for (int pt = 0; pt < 100; pt++)
      {
      // convert world coordinates (wcs) into image pixel
      double imgX, imgY;
      int    offScale;
      wcs2pix(fWcs, axisA1 + pt * deltaAxisA, axisB1 + pt * deltaAxisB,
              &imgX, &imgY, &offScale);

      // convert image pixels into screen coordinates
      screenX[numPt] = (imgX - fZoomOffX) * xconv;
      screenY[numPt] = (imgY - fZoomOffY) * yconv + bottom_gap;

      // is this point outside of the actually drawn image?
      bool   out = CoordsOut(screenX[numPt], screenY[numPt], right_gap, bottom_gap);

      if      ((status == 0 || status == 3) && !out)  status = 1;
      else if (status == 1 && !out)                   status = 2;
      else if ((status == 1 || status == 2) && out)   status = 3;
      else if (status == 3 && out)                    status = 0;

      if (status == 0)
         {
         // we are still outside of the image
         prevScreenX = screenX[numPt];
         prevScreenY = screenY[numPt];
         continue;
         }
      else if (status == 1 && pt != 0)
         // we are in for the first time, but were at least once outside
         {
         BorderCoords(screenX[numPt], screenY[numPt], prevScreenX, prevScreenY, 
                      right_gap, bottom_gap);
         screenX[numPt+1] = screenX[numPt];
         screenY[numPt+1] = screenY[numPt];
         screenX[numPt]   = prevScreenX;
         screenY[numPt]   = prevScreenY;
         numPt++;
         }
      else if (status == 3)
         // we are out again
         BorderCoords(screenX[numPt-1], screenY[numPt-1], 
                      screenX[numPt], screenY[numPt], right_gap, bottom_gap);

      numPt++;
      if (!labelDrawn && status == 2)
         {
         double dx = (screenX[0] - screenX[numPt-1]) / right_gap;
         double dy = (screenY[0] - screenY[numPt-1]) / (1 - bottom_gap);
         if (dx * dx + dy * dy > TAttText::GetTextSize() * TAttText::GetTextSize() * 15)
            {
            // draw the label
            dx = (screenX[0] - screenX[1]) / xconv;
            dy = (screenY[0] - screenY[1]) / yconv;

            double angle;
            if (fabs(dx) < 1e-10)
               angle = 90;
            else
               angle = atan( dy / dx ) * 57.295;
            if (screenX[1] < screenX[0])
               angle += 180;

            gVirtualX->SetTextAngle(angle);
            if (gVirtualPS)
               gVirtualPS->SetTextAngle(angle);

            double labelValue = (axisA1 == axisA2) ? 
                                (axisA1 >= 360 ? axisA1 - 360 : axisA1) : axisB1;
            gPad->PaintText(screenX[0], screenY[0], 
                            GetLabel(labelValue, scaleType) );

            labelDrawn = true;
            }
         }

      if (status == 3)
         {
         gPad->PaintPolyLine(numPt, screenX, screenY, "C");
         numPt = 0;
         }
      }

   gPad->PaintPolyLine(numPt, screenX, screenY, "C");


}
//_____________________________________________________________________________
Bool_t AstroImage::ScrPixel2ImgPixel(Int_t scrX, Int_t scrY, 
                                     Double_t & imgX, Double_t & imgY)
{
// Converts screen pixels into image pixels. It takes the zooming and
// scaling into account.
// Returns kFALSE if scrX or scrY is outside of the actually drawn image
// in the screen.

   imgX = scrX - gPad->XtoAbsPixel(0);
   imgY = scrY - gPad->YtoAbsPixel(1);
   if ( imgX < 0 || imgY < 0) return kFALSE;

   if (imgX > GetScaledWidth() || imgY > GetScaledHeight())
      return kFALSE;

   imgY = GetScaledHeight() - 1 - imgY;
   // convert to original image size and take zooming into account
   if (fScaledImage) 
      {
      imgX = (imgX / (Double_t)GetScaledWidth()  * fZoomWidth ) + fZoomOffX;
      imgY = (imgY / (Double_t)GetScaledHeight() * fZoomHeight) + fZoomOffY;
      }

   return kTRUE;
}
//_____________________________________________________________________________
Double_t AstroImage::GetPixelSizeX()
{
// Returns the width of one pixel in units of the width of the frame.

   return gPad->AbsPixeltoX(GetScaledWidth()  + gPad->XtoAbsPixel(0)) / 
          fZoomWidth;
}
//_____________________________________________________________________________
Double_t AstroImage::GetPixelSizeY()
{
// Returns the height of one pixel in units of the width of the frame.

   return -1.0 * gPad->AbsPixeltoY(GetScaledHeight() + gPad->YtoAbsPixel(0)) / 
          fZoomHeight;
}
//_____________________________________________________________________________
Bool_t AstroImage::ImgPixel2FrameCoord(Double_t imgX, Double_t imgY, 
                                       Double_t * frX, Double_t * frY)
{
// Convertes a image pixel (imgX / imgY) into the frame coordinates on the pad.
// The frame coordinates are in the range from 0 to 1.0.
// kTRUE is returned if the pixel is visible on the screen.
// kFALSE if the pixel is actually not drawn.

   Double_t xconv = GetPixelSizeX();
   Double_t yconv = GetPixelSizeY();
   Double_t bottom_gap = 1 + gPad->AbsPixeltoY(GetScaledHeight() + gPad->YtoAbsPixel(0));

   *frX = (imgX - fZoomOffX) * xconv;
   *frY = (imgY - fZoomOffY) * yconv + bottom_gap;

   return imgX  < fZoomOffX + fZoomWidth  &&
          imgX  > fZoomOffX               &&
          imgY  < fZoomOffY + fZoomHeight &&
          imgY  > fZoomOffY;

}
//_____________________________________________________________________________
void AstroImage::SetTitle(const char * text)
{
// Writes the text in a TPaveText. A new TPaveText is drawn on the current 
// TPad if there is'n any yet.

   // update an already existing PaveText with new text
   TIter next(gPad->GetListOfPrimitives());
   while (TObject* obj = next()) 
      {
      if (obj->IsA() == TPaveText::Class()) 
         {
         ((TPaveText*)obj)->Clear();
         ((TPaveText*)obj)->AddText(text);
         return;
         }
      }

   // there exist no TPaveText jet in the pad. Create a new one
   TPaveText * pave = new TPaveText(0.1, 0.94, 0.9, 1.02);
   pave->AddText(text);
   pave->Draw();

}
//_____________________________________________________________________________
void AstroImage::AddRegion(const char * type, const char * text)
{
// Adds a new region to this image. It is drawn on the current TPad taken 
// into account the zoomed image.
// 
// Following types are currently supported:
//    name           function parameer "type" must  
//                   start with these characters
//
//    circle            ci
//    ellipse           el
//    line              li
//    point             po
//
// The attributres of all regions can be modified with the mouse. 
// Specially the point region can be any of the TMarker types.
// 
// Each reagion can have its own text which is displayd above the
// region. The attributes of the text cannot be modified.

   Double_t imgX, imgY;

   if (!ScrPixel2ImgPixel(gPad->GetEventX(), gPad->GetEventY(), imgX, imgY)) 
      return;

   TString ty(type);

   if (ty.BeginsWith("ci", TString::kIgnoreCase))
      {
      AstroCircle * astroCircle = new AstroCircle(imgX, imgY, text);
      astroCircle->Draw();
      }

   else if (ty.BeginsWith("el", TString::kIgnoreCase))
      {
      AstroEllipse * astroEllipse = new AstroEllipse(imgX, imgY, text);
      astroEllipse->Draw();
      }

   else if (ty.BeginsWith("li", TString::kIgnoreCase))
      {
      AstroLine * astroLine = new AstroLine(imgX, imgY, text);
      astroLine->Draw();
      }

   else if (ty.BeginsWith("po", TString::kIgnoreCase))
      {
      AstroPoint * astroPoint = new AstroPoint(imgX, imgY, text);
      astroPoint->Draw();
      }
}
//_____________________________________________________________________________
void AstroImage::AddText(const char * text)
{
// Adds a text region to this image. It is drawn on the current TPad taken 
// into account the zoomed image.
// The Text is actually a TLatex. Therefore supported Latex syntax can be
// used.

   Double_t imgX, imgY;

   if (!ScrPixel2ImgPixel(gPad->GetEventX(), gPad->GetEventY(), imgX, imgY)) 
      return;

   AstroText * astroText = new AstroText(imgX, imgY, text);
   astroText->SetTextAlign(22);
   astroText->SetTextSizePixels(20);
   astroText->Draw();
}
//_____________________________________________________________________________
void AstroImage::CoordinateSystem(const char * coordSystem)
{
// Set the coordinate sytem. 
// Following systems are supported:
//    name           function parameer "coordSystem" must  
//                   start with these characters
//
//    ICRS              i or I
//    FK4               fk4 or FK4
//    FK5               fk5 or FK5
//    PHYSICAL          p or P
//    B1950             b or B
//    J2000             j or J
//    GALACTIC          g or G
//    ECLIPTIC          e or E
//    LINEAR            l or L

   char wcsSystem[100];

   if (coordSystem[0] == 'I' || coordSystem[0] == 'i')   
      strcpy(wcsSystem, "ICRS");

   else if (strncmp("fk4", coordSystem, 3) == 0 ||
            strncmp("FK4", coordSystem, 3) == 0    )
      strcpy(wcsSystem, "FK4");

   else if (strncmp("fk5", coordSystem, 3) == 0 ||
            strncmp("FK5", coordSystem, 3) == 0    )
      strcpy(wcsSystem, "FK5");

   else if (coordSystem[0] == 'P' || coordSystem[0] == 'p')   
      strcpy(wcsSystem, "PHYSICAL");

   else if (coordSystem[0] == 'B' || coordSystem[0] == 'b')   
      strcpy(wcsSystem, "B1950");

   else if (coordSystem[0] == 'J' || coordSystem[0] == 'j')   
      strcpy(wcsSystem, "J2000");

   else if (coordSystem[0] == 'G' || coordSystem[0] == 'g')   
      strcpy(wcsSystem, "GALACTIC");

   else if (coordSystem[0] == 'E' || coordSystem[0] == 'e')   
      strcpy(wcsSystem, "ECLIPTIC");

   else if (coordSystem[0] == 'L' || coordSystem[0] == 'l')   
      strcpy(wcsSystem, "LINEAR");

   else  return;


   wcsoutinit(fWcs, wcsSystem);
   wcsininit(fWcs, wcsSystem);

}
//_____________________________________________________________________________
void AstroImage::WriteRegion()
{
// Writes all regions of the image to an ASCII region file.

   if (!iswcs(fWcs)) return;
   
   const char *filetypes[] = {"region file", "*.reg", 0, 0};
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), 
						  kFDSave, &fi);
   if (fi.fFilename == NULL) return;

   FILE * file = fopen(fi.fFilename, "w");
   if (file == NULL) return;

   TIter next(gPad->GetListOfPrimitives());
   while(TObject *obj = next())
      {
      if (obj->IsA() == AstroText::Class() ) 
         ((AstroText*)obj)->WriteRegion(file, fWcs);
      else if (obj->IsA() == AstroLine::Class() ) 
         ((AstroLine*)obj)->WriteRegion(file, fWcs);
      else if (obj->IsA() == AstroCircle::Class() ) 
         ((AstroCircle*)obj)->WriteRegion(file, fWcs);
      else if (obj->IsA() == AstroEllipse::Class() ) 
         ((AstroEllipse*)obj)->WriteRegion(file, fWcs);
      else if (obj->IsA() == AstroPoint::Class() ) 
         ((AstroPoint*)obj)->WriteRegion(file, fWcs);
      }   
   
   fclose(file);      

}
//_____________________________________________________________________________
void AstroImage::ReadRegion()
{
// Reads regions from an ASCII region file and displays them.
// See thea astroimage user manual for more information about 
// the format of a region file.

   if (!iswcs(fWcs)) return;

   const char *filetypes[] = {"region file", "*.reg", 0, 0};
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), 
						  kFDOpen, &fi);

   if (fi.fFilename == NULL) return;

   FILE * file = fopen(fi.fFilename, "r");
   if (file == NULL) return;
   
   char line[2048];
   char currentSystem[30];    // current coordinate system during reading
                              // the region file. 

   RegionProperties globalProperties;

   strcpy(currentSystem, "PHYSICAL");  // default coordinate system

   // read line by line
   while (fgets(line, 2048, file) != NULL)
      {
      // skipp comment lines
      if (line[0] == '#')  
         continue;

      // regions are seperated by ;
      char * next = line;
      char * semicolon;
      do {
         semicolon = strchr(next, ';');
         if (semicolon) *semicolon = 0;

         Read1Region(next, currentSystem, globalProperties);

         if (semicolon) next = semicolon + 1;
         } while (semicolon);

      }
   
   fclose(file);      
}
//_____________________________________________________________________________

