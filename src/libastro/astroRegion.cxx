// ////////////////////////////////////////////////////////////////////////////
//
//  File:      astroRegion.cxx
//
//  Version:   1.0
//
//  Author:    Reiner Rohlfs (GADC)
//
//  History:   1.0   26.04.04  first released version
//
// ////////////////////////////////////////////////////////////////////////////
#include <math.h>

#include <Buttons.h>
#include <TPad.h>
#include <TMath.h>
#include <TList.h>

#include "astroRegion.h"
#include "astroImage.h"
#include "wcs.h"

ClassImp(AstroRegion)
ClassImp(AstroText)
ClassImp(AstroCircle)
ClassImp(AstroEllipse)
ClassImp(AstroLine)
ClassImp(AstroPoint)

//_____________________________________________________________________________
//  This class should not be used directly by an application. To add a new
//  region to an image call the AstroImage::AddRegion() function.


const char * COLOR [] = {
"black",
"black",
"red",
"green",
"blue",
"yellow",
"magenta",
"cyan",
"green",
"blue",
"white"
};


//_____________________________________________________________________________
AstroRegion::AstroRegion(Double_t imgX, Double_t imgY, const char * text)
{
// Base class for all region classes.

   fImgX = imgX;
   fImgY = imgY;
   fRegionText = text;
}
//_____________________________________________________________________________
AstroImage * AstroRegion::ExecuteEvent(Int_t event, Double_t fX, Double_t fY)
{
   static Double_t startX, startY;

   // we want to change the position only if the text was moved. We store
   // the position of the text when the mouse was pressed
   if (event == kButton1Down) 
      {
      startX = fX;
      startY = fY;
      }

   // if the text was moved we change the image pixel of the text. 
   // We use a function of the image on the same pad to calculate the
   // new image pixels. The image pixels depend on zooming and scaling
   // of the image which the AstroText object does not know.
   else if (event == kButton1Up && 
            (fabs(startX - fX) > 0.005 || 
             fabs(startY - fY) > 0.005)    )
      {
      TIter next(gPad->GetListOfPrimitives());
      while(TObject *obj = next())
         {
         if (obj->IsA()->InheritsFrom(AstroImage::Class()))
            {
            Double_t newX, newY;
            if ( ((AstroImage*)obj)->ScrPixel2ImgPixel( gPad->XtoAbsPixel(fX), 
                                                        gPad->YtoAbsPixel(fY), newX, newY) )
               {
               // new position only if the new position is not outside of 
               // the image
               fImgX = newX;
               fImgY = newY;
               }
            return (AstroImage*)obj;
            }
         }
      }

   return NULL;
}
//_____________________________________________________________________________
void AstroRegion::WriteRegion(FILE * file, WorldCoor * wcs)
{
// Writes the coordinate system to the region file.

   fprintf(file, "%s;", getwcsin(wcs));
}

//_____________________________________________________________________________
//_____________________________________________________________________________
AstroText::AstroText(Double_t imgX, Double_t imgY, const char * text)
   : TLatex(0, 0, text), AstroRegion(imgX, imgY, "")
{
// A region of just a text.

   // default color is white
   SetTextColor(10);
}
//_____________________________________________________________________________
void AstroText::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

   // first move, rotate and resize the text
   TLatex::ExecuteEvent(event, px, py);

   AstroRegion::ExecuteEvent(event, fX, fY);
}
//_____________________________________________________________________________
void  AstroText::Paint(Option_t * option)
{
// Draws the text in the pad.

   TIter next(gPad->GetListOfPrimitives());
   while(TObject *obj = next())
      {
      if (obj->IsA()->InheritsFrom(AstroImage::Class()))
         {
         if ( ((AstroImage*)obj)->ImgPixel2FrameCoord(fImgX, fImgY, &fX, &fY)  )
            {
            TLatex::Paint(option);
            }
         break;
         }
      }
}
//_____________________________________________________________________________
void AstroText::WriteRegion(FILE * file, WorldCoor * wcs)
{
// Writes this region and its attributres into a region file.

   AstroRegion::WriteRegion(file, wcs);

   double axisA, axisB;

   pix2wcs(wcs, fImgX, fImgY, &axisA, &axisB);
   fprintf(file, "text (%9.4f, %9.4f) # textangle = %d color = %s text = {%s}\n", 
          axisA, axisB, (int)GetTextAngle(), COLOR[GetTextColor() % 11], GetTitle());

}

//_____________________________________________________________________________
//_____________________________________________________________________________

AstroCircle::AstroCircle(Double_t imgX, Double_t imgY, const char * text)
   : TEllipse(0, 0, 0.10), AstroRegion(imgX, imgY, text)
{
// A circle region. The size of the circle can be changed 
// with the mouse.

   // default color is white
   SetLineColor(10);
   SetLineWidth(2);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

   fImgR = 20;
}

//_____________________________________________________________________________

void AstroCircle::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

// The code is copied from TEllipse::ExecuteEvent and modified to 
// keep a circle

const Double_t kPI = 3.14159265358979323846;

   // This is slightly modified code of TEllipse::ExecuteEvent
   Int_t kMaxDiff = 10;
   const Int_t kMinSize = 15;
   const Int_t np = 40;
   static Int_t x[np+2], y[np+2];
   static Int_t px1,py1,npe,R1,R2,sav1,sav2;
   static Int_t pxold, pyold;
   static Int_t sig,impair;
   Int_t i, dpx, dpy;
   Double_t angle,dx,dy,dphi,ct,st,fTy,fBy,fLx,fRx;
   static Bool_t T, L, R, B, INSIDE;
   static Int_t Tx,Ty,Lx,Ly,Rx,Ry,Bx,By;

   if (!gPad->IsEditable()) return;

   switch (event) {

   case kButton1Down:
      gVirtualX->SetLineColor(-1);
      TAttLine::Modify();
      dphi = (fPhimax-fPhimin)*kPI/(180*np);
      ct   = TMath::Cos(kPI*fTheta/180);
      st   = TMath::Sin(kPI*fTheta/180);
      for (i=0;i<np;i++) {
         angle = fPhimin*kPI/180 + Double_t(i)*dphi;
         dx    = fR1*TMath::Cos(angle);
         dy    = fR2*TMath::Sin(angle);
         x[i]  = gPad->XtoAbsPixel(fX1 + dx*ct - dy*st);
         y[i]  = gPad->YtoAbsPixel(fY1 + dx*st + dy*ct);
      }
      if (fPhimax-fPhimin >= 360 ) {
         x[np] = x[0];
         y[np] = y[0];
         npe = np;
      } else {
         x[np]   = gPad->XtoAbsPixel(fX1);
         y[np]   = gPad->YtoAbsPixel(fY1);
         x[np+1] = x[0];
         y[np+1] = y[0];
         npe = np + 1;
      }
      impair = 0;
      px1 = gPad->XtoAbsPixel(fX1);
      py1 = gPad->YtoAbsPixel(fY1);
      Tx = Bx = px1;
      Ly = Ry = py1;
      Ty = gPad->YtoAbsPixel(fR2+fY1);
      By = gPad->YtoAbsPixel(-fR2+fY1);
      Lx = gPad->XtoAbsPixel(-fR1+fX1);
      Rx = gPad->XtoAbsPixel(fR1+fX1);
      R2 = (By-Ty)/2;
      R1 = (Rx-Lx)/2;
      gVirtualX->DrawLine(Rx+4, py1+4, Rx-4, py1+4);
      gVirtualX->DrawLine(Rx-4, py1+4, Rx-4, py1-4);
      gVirtualX->DrawLine(Rx-4, py1-4, Rx+4, py1-4);
      gVirtualX->DrawLine(Rx+4, py1-4, Rx+4, py1+4);
      gVirtualX->DrawLine(Lx+4, py1+4, Lx-4, py1+4);
      gVirtualX->DrawLine(Lx-4, py1+4, Lx-4, py1-4);
      gVirtualX->DrawLine(Lx-4, py1-4, Lx+4, py1-4);
      gVirtualX->DrawLine(Lx+4, py1-4, Lx+4, py1+4);
      gVirtualX->DrawLine(px1+4, By+4, px1-4, By+4);
      gVirtualX->DrawLine(px1-4, By+4, px1-4, By-4);
      gVirtualX->DrawLine(px1-4, By-4, px1+4, By-4);
      gVirtualX->DrawLine(px1+4, By-4, px1+4, By+4);
      gVirtualX->DrawLine(px1+4, Ty+4, px1-4, Ty+4);
      gVirtualX->DrawLine(px1-4, Ty+4, px1-4, Ty-4);
      gVirtualX->DrawLine(px1-4, Ty-4, px1+4, Ty-4);
      gVirtualX->DrawLine(px1+4, Ty-4, px1+4, Ty+4);
      // No break !!!

   case kMouseMotion:
      px1 = gPad->XtoAbsPixel(fX1);
      py1 = gPad->YtoAbsPixel(fY1);
      Tx = Bx = px1;
      Ly = Ry = py1;
      Ty = gPad->YtoAbsPixel(fR2+fY1);
      By = gPad->YtoAbsPixel(-fR2+fY1);
      Lx = gPad->XtoAbsPixel(-fR1+fX1);
      Rx = gPad->XtoAbsPixel(fR1+fX1);
      T = L = R = B = INSIDE = kFALSE;
      if ((TMath::Abs(px - Tx) < kMaxDiff) &&
          (TMath::Abs(py - Ty) < kMaxDiff)) {             // top edge
         T = kTRUE;
         gPad->SetCursor(kTopSide);
      }
      else
      if ((TMath::Abs(px - Bx) < kMaxDiff) &&
          (TMath::Abs(py - By) < kMaxDiff)) {             // bottom edge
         B = kTRUE;
         gPad->SetCursor(kBottomSide);
      }
      else
      if ((TMath::Abs(py - Ly) < kMaxDiff) &&
          (TMath::Abs(px - Lx) < kMaxDiff)) {             // left edge
         L = kTRUE;
         gPad->SetCursor(kLeftSide);
      }
      else
      if ((TMath::Abs(py - Ry) < kMaxDiff) &&
          (TMath::Abs(px - Rx) < kMaxDiff)) {             // right edge
         R = kTRUE;
         gPad->SetCursor(kRightSide);
      }
      else {INSIDE= kTRUE; gPad->SetCursor(kMove); }
      pxold = px;  pyold = py;

      break;

   case kButton1Motion:
      gVirtualX->DrawLine(Rx+4, py1+4, Rx-4, py1+4);
      gVirtualX->DrawLine(Rx-4, py1+4, Rx-4, py1-4);
      gVirtualX->DrawLine(Rx-4, py1-4, Rx+4, py1-4);
      gVirtualX->DrawLine(Rx+4, py1-4, Rx+4, py1+4);
      gVirtualX->DrawLine(Lx+4, py1+4, Lx-4, py1+4);
      gVirtualX->DrawLine(Lx-4, py1+4, Lx-4, py1-4);
      gVirtualX->DrawLine(Lx-4, py1-4, Lx+4, py1-4);
      gVirtualX->DrawLine(Lx+4, py1-4, Lx+4, py1+4);
      gVirtualX->DrawLine(px1+4, By+4, px1-4, By+4);
      gVirtualX->DrawLine(px1-4, By+4, px1-4, By-4);
      gVirtualX->DrawLine(px1-4, By-4, px1+4, By-4);
      gVirtualX->DrawLine(px1+4, By-4, px1+4, By+4);
      gVirtualX->DrawLine(px1+4, Ty+4, px1-4, Ty+4);
      gVirtualX->DrawLine(px1-4, Ty+4, px1-4, Ty-4);
      gVirtualX->DrawLine(px1-4, Ty-4, px1+4, Ty-4);
      gVirtualX->DrawLine(px1+4, Ty-4, px1+4, Ty+4);
      for (i=0;i<npe;i++) gVirtualX->DrawLine(x[i], y[i], x[i+1], y[i+1]);
      if (T) {
         sav1 = py1;
         sav2 = R2;
         py1 += (py - pyold)/2;
         R2 -= (py - pyold)/2;
         if (TMath::Abs(pyold-py)%2==1) impair++;
         if (py-pyold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; py1 += sig; R2 -= sig;}
         if (py1 > By-kMinSize) {py1 = sav1; R2 = sav2; py = pyold;}
         R1 = R2;
      }
      if (B) {
         sav1 = py1;
         sav2 = R2;
         py1 += (py - pyold)/2;
         R2 += (py - pyold)/2;
         if (TMath::Abs(pyold-py)%2==1) impair++;
         if (py-pyold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; py1 += sig; R2 += sig;}
         if (py1 < Ty+kMinSize) {py1 = sav1; R2 = sav2; py = pyold;}
         R1 = R2;
      }
      if (L) {
         sav1 = px1;
         sav2 = R1;
         px1 += (px - pxold)/2;
         R1 -= (px - pxold)/2;
         if (TMath::Abs(pxold-px)%2==1) impair++;
         if (px-pxold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; px1 += sig; R1 -= sig;}
         if (px1 > Rx-kMinSize) {px1 = sav1; R1 = sav2; px = pxold;}
         R2 = R1;
      }
      if (R) {
         sav1 = px1;
         sav2 = R1;
         px1 += (px - pxold)/2;
         R1 += (px - pxold)/2;
         if (TMath::Abs(pxold-px)%2==1) impair++;
         if (px-pxold>0) sig=+1;
         else sig=-1;
         if (impair==2) { impair = 0; px1 += sig; R1 += sig;}
         if (px1 < Lx+kMinSize) {px1 = sav1; R1 = sav2; px = pxold;}
         R2 = R1;
      }
      if (T || B || L || R) {
         gVirtualX->SetLineColor(-1);
         TAttLine::Modify();
         dphi = (fPhimax-fPhimin)*kPI/(180*np);
         ct   = TMath::Cos(kPI*fTheta/180);
         st   = TMath::Sin(kPI*fTheta/180);
         for (i=0;i<np;i++) {
            angle = fPhimin*kPI/180 + Double_t(i)*dphi;
            dx    = R1*TMath::Cos(angle);
            dy    = R2*TMath::Sin(angle);
            x[i]  = px1 + Int_t(dx*ct - dy*st);
            y[i]  = py1 + Int_t(dx*st + dy*ct);
         }
         if (fPhimax-fPhimin >= 360 ) {
            x[np] = x[0];
            y[np] = y[0];
            npe = np;
         } else {
            x[np]   = px1;
            y[np]   = py1;
            x[np+1] = x[0];
            y[np+1] = y[0];
            npe = np + 1;
         }
         for (i=0;i<npe;i++) gVirtualX->DrawLine(x[i], y[i], x[i+1], y[i+1]);
      }
      if (INSIDE) {
          dpx  = px-pxold;  dpy = py-pyold;
          px1 += dpx; py1 += dpy;
          for (i=0;i<=npe;i++) { x[i] += dpx; y[i] += dpy;}
          for (i=0;i<npe;i++) gVirtualX->DrawLine(x[i], y[i], x[i+1], y[i+1]);
      }
      Tx = Bx = px1;
      Rx = px1+R1;
      Lx = px1-R1;
      Ry = Ly = py1;
      Ty = py1-R2;
      By = py1+R2;
      gVirtualX->DrawLine(Rx+4, py1+4, Rx-4, py1+4);
      gVirtualX->DrawLine(Rx-4, py1+4, Rx-4, py1-4);
      gVirtualX->DrawLine(Rx-4, py1-4, Rx+4, py1-4);
      gVirtualX->DrawLine(Rx+4, py1-4, Rx+4, py1+4);
      gVirtualX->DrawLine(Lx+4, py1+4, Lx-4, py1+4);
      gVirtualX->DrawLine(Lx-4, py1+4, Lx-4, py1-4);
      gVirtualX->DrawLine(Lx-4, py1-4, Lx+4, py1-4);
      gVirtualX->DrawLine(Lx+4, py1-4, Lx+4, py1+4);
      gVirtualX->DrawLine(px1+4, By+4, px1-4, By+4);
      gVirtualX->DrawLine(px1-4, By+4, px1-4, By-4);
      gVirtualX->DrawLine(px1-4, By-4, px1+4, By-4);
      gVirtualX->DrawLine(px1+4, By-4, px1+4, By+4);
      gVirtualX->DrawLine(px1+4, Ty+4, px1-4, Ty+4);
      gVirtualX->DrawLine(px1-4, Ty+4, px1-4, Ty-4);
      gVirtualX->DrawLine(px1-4, Ty-4, px1+4, Ty-4);
      gVirtualX->DrawLine(px1+4, Ty-4, px1+4, Ty+4);
      pxold = px;
      pyold = py;
      break;

   case kButton1Up:
      fX1 = gPad->AbsPixeltoX(px1);
      fY1 = gPad->AbsPixeltoY(py1);
      fBy = gPad->AbsPixeltoY(py1+R2);
      fTy = gPad->AbsPixeltoY(py1-R2);
      fLx = gPad->AbsPixeltoX(px1+R1);
      fRx = gPad->AbsPixeltoX(px1-R1);
      fR1 = TMath::Abs(fRx-fLx)/2;
      fR2 = TMath::Abs(fTy-fBy)/2;
      gPad->Modified(kTRUE);
      gVirtualX->SetLineColor(-1);
   }

   AstroImage * img = AstroRegion::ExecuteEvent(event, fX1, fY1);
   if (img)
      {
      fImgR = fR1 / img->GetPixelSizeX();
      }
}
//_____________________________________________________________________________
void AstroCircle::Paint(Option_t * option) 
{
// Draws the circle and it text.

   TIter next(gPad->GetListOfPrimitives());
   while(TObject *obj = next())
      {
      if (obj->IsA()->InheritsFrom(AstroImage::Class()))
         {
         if ( ((AstroImage*)obj)->ImgPixel2FrameCoord(fImgX, fImgY, &fX1, &fY1) )
            {
            fR1 = fImgR * ((AstroImage*)obj)->GetPixelSizeX();
            fR2 = fImgR * ((AstroImage*)obj)->GetPixelSizeY();

            TEllipse::Paint(option);

            if (!fRegionText.IsNull())
               {
               SetTextColor(GetLineColor());
               TAttText::Modify();
               gPad->PaintText(fX1, fY1 + 1.1 * fR2, fRegionText.Data() );
               }

            }
         break;
         }
      }

}
//_____________________________________________________________________________
void AstroCircle::WriteRegion(FILE * file, WorldCoor * wcs)
{
// Writes this region and its attributres into a region file.

   AstroRegion::WriteRegion(file, wcs);

   double axisA, axisB;

   pix2wcs(wcs, fImgX, fImgY, &axisA, &axisB);
   fprintf(file, "circle (%9.4f, %9.4f, %9.4fi) # color = %s", 
          axisA, axisB, fImgR, COLOR[GetLineColor() % 11] );

   if (!fRegionText.IsNull())
      fprintf(file, " text = {%s}", fRegionText.Data() );

   fprintf(file, "\n");   
}

//_____________________________________________________________________________
//_____________________________________________________________________________
AstroEllipse::AstroEllipse(Double_t imgX, Double_t imgY, const char * text)
   : TEllipse(0, 0, 0.10), AstroRegion(imgX, imgY, text)
{
// An ellipse region. The size of the ellipse can be changed 
// with the mouse.

   // default color is white
   SetLineColor(10);
   SetLineWidth(2);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

   fImgR1 = 30;
   fImgR2 = 20;
}
//_____________________________________________________________________________
void AstroEllipse::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

   // first move, rotate and resize the ellipse
   TEllipse::ExecuteEvent(event, px, py);

   AstroImage * img = AstroRegion::ExecuteEvent(event, fX1, fY1);
   if (img)
      {
      fImgR1 = fR1 / img->GetPixelSizeX();
      fImgR2 = fR2 / img->GetPixelSizeY();
      }
}
//_____________________________________________________________________________
void  AstroEllipse::Paint(Option_t * option)
{
// Draws the ellipse and it text.

   TIter next(gPad->GetListOfPrimitives());
   while(TObject *obj = next())
      {
      if (obj->IsA()->InheritsFrom(AstroImage::Class()))
         {
         if ( ((AstroImage*)obj)->ImgPixel2FrameCoord(fImgX, fImgY, &fX1, &fY1) )
            {
            fR1 = fImgR1 * ((AstroImage*)obj)->GetPixelSizeX();
            fR2 = fImgR2 * ((AstroImage*)obj)->GetPixelSizeY();

            TEllipse::Paint(option);

            if (!fRegionText.IsNull())
               {
               SetTextColor(GetLineColor());
               TAttText::Modify();
               gPad->PaintText(fX1, fY1 + 1.1 * fR2, fRegionText.Data() );
               }
            }
         break;
         }
      }
}
//_____________________________________________________________________________
void AstroEllipse::WriteRegion(FILE * file, WorldCoor * wcs)
{
// Writes this region and its attributres into a region file.

   AstroRegion::WriteRegion(file, wcs);

   double axisA, axisB;

   pix2wcs(wcs, fImgX, fImgY, &axisA, &axisB);
   fprintf(file, "ellipse (%9.4f, %9.4f, %9.4fi, %9.4fi, %5.1f) # color = %s", 
          axisA, axisB, fImgR1, fImgR2, GetTheta(), COLOR[GetLineColor() % 11] );

   if (!fRegionText.IsNull())
      fprintf(file, " text = {%s}", fRegionText.Data() );

   fprintf(file, "\n");   
}

//_____________________________________________________________________________
//_____________________________________________________________________________
AstroLine::AstroLine(Double_t imgX, Double_t imgY, const char * text)
   : TArrow(0.2, 0.5, 0.8, 0.5), AstroRegion(imgX - 10, imgY, text)
{
// A  line region. The length and orientation of the line can be changed
// with the mouse.

   fImgX2 = imgX + 10;
   fImgY2 = imgY;

   // default color is white
   SetLineColor(10);
   SetLineWidth(2);
   SetOption("");
   SetArrowSize(0.02);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

}

//_____________________________________________________________________________

void AstroLine::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

   // first move, rotate and resize the ellipse
   TLine::ExecuteEvent(event, px, py);

   AstroImage * img = AstroRegion::ExecuteEvent(event, fX1, fY1);


   static Double_t startX, startY;

   // we want to change the position only if the text was moved. We store
   // the position of the text when the mouse was pressed
   if (event == kButton1Down) 
      {
      startX = fX2;
      startY = fY2;
      }

   // if the text was moved we change the image pixel of the text. 
   // We use a function of the image on the same pad to calculate the
   // new image pixels. The image pixels depend on zooming and scaling
   // of the image which the AstroText object does not know.
   else if (event == kButton1Up && 
            (fabs(startX - fX2) > 0.005 || 
             fabs(startY - fY2) > 0.005)    )
      {
      TIter next(gPad->GetListOfPrimitives());
      while(TObject *obj = next())
         {
         if (obj->IsA()->InheritsFrom(AstroImage::Class()))
            {
            Double_t newX, newY;
            if ( ((AstroImage*)obj)->ScrPixel2ImgPixel( gPad->XtoAbsPixel(fX2), 
                                                        gPad->YtoAbsPixel(fY2), newX, newY) )
               {
               // new position only if the new position is not outside of 
               // the image
               fImgX2 = newX;
               fImgY2 = newY;
               }
            break;
            }
         }
      }

}

//_____________________________________________________________________________

void  AstroLine::Paint(Option_t * option)
{
// Draws the line and it text.

   TIter next(gPad->GetListOfPrimitives());
   while(TObject *obj = next())
      {
      if (obj->IsA()->InheritsFrom(AstroImage::Class()))
         {
         if ( ((AstroImage*)obj)->ImgPixel2FrameCoord(fImgX,  fImgY,  &fX1, &fY1) &&
              ((AstroImage*)obj)->ImgPixel2FrameCoord(fImgX2, fImgY2, &fX2, &fY2)   )
            {

            TArrow::Paint(option);

            if (!fRegionText.IsNull())
               {
               double angle;
               if (fabs(fImgX - fImgX2) > 0.5)
                  angle = TMath::ATan( (fImgY - fImgY2) / (fImgX - fImgX2) );
               else
                  angle =  3.1416 / 2;  // 90 deg
               TAttText::SetTextAngle(angle / 3.1416 * 180);

               SetTextColor(GetLineColor());
               TAttText::Modify();
               gPad->PaintText( (fX1 + fX2) / 2  - 0.01 * TMath::Sin(angle), 
                                (fY1 + fY2) / 2  + 0.01 * TMath::Cos(angle), fRegionText.Data() );
               }

            }
         break;
         }
      }
}
//_____________________________________________________________________________
void AstroLine::WriteRegion(FILE * file, WorldCoor * wcs)
{
// writes this region and its attributres into a region file

   AstroRegion::WriteRegion(file, wcs);

   double axisA, axisB;

   pix2wcs(wcs, fImgX, fImgY, &axisA, &axisB);
   fprintf(file, "line (%9.4f, %9.4f, ", axisA, axisB);

   pix2wcs(wcs, fImgX2, fImgY2, &axisA, &axisB);
   fprintf(file, "%9.4f, %9.4f) # color = %s", 
           axisA, axisB, COLOR[GetLineColor() % 11]);

   TObjLink *lnk = (TObjLink*)gPad->GetListOfPrimitives()->FirstLink();
   while (lnk) 
      {
      if (this == lnk->GetObject())
         {
         TString opt =  lnk->GetOption();
         fprintf(file, " line = %d %d", opt.Contains("<") ? 1 : 0,
                                        opt.Contains(">") ? 1 : 0 );
         break;
         }
      lnk = (TObjOptLink*)lnk->Next();
      }

   if (!fRegionText.IsNull())
      fprintf(file, " text = {%s}", fRegionText.Data() );

   fprintf(file, "\n");
}
//_____________________________________________________________________________
//_____________________________________________________________________________

AstroPoint::AstroPoint(Double_t imgX, Double_t imgY, const char * text)
   : TMarker(0.2, 0.5, 24), AstroRegion(imgX - 10, imgY, text)
{
// A point is actually a ROOT marker. The type of the point can be changed
// with the mouse.

   // default color is white
   SetMarkerColor(10);
   SetMarkerSize(1.5);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

}
//_____________________________________________________________________________
void AstroPoint::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

   // first move, rotate and resize the ellipse
   TMarker::ExecuteEvent(event, px, py);

   AstroRegion::ExecuteEvent(event, fX, fY);

}
//_____________________________________________________________________________
void  AstroPoint::Paint(Option_t * option)
{
// Draws the marker and its text.

   TIter next(gPad->GetListOfPrimitives());
   while(TObject *obj = next())
      {
      if (obj->IsA()->InheritsFrom(AstroImage::Class()))
         {
         if ( ((AstroImage*)obj)->ImgPixel2FrameCoord(fImgX, fImgY, &fX, &fY)  )
            {
            TMarker::Paint(option);

            if (!fRegionText.IsNull())
               {
               SetTextColor(GetMarkerColor());
               TAttText::Modify();
               gPad->PaintText(fX, fY + 0.02, fRegionText.Data() );
               }
            }
         break;
         }
      }
}
//_____________________________________________________________________________
static const char * MarkerStyleToString(Style_t markerStyle)
{
   static char style[10];
   switch (markerStyle)
      {
      case 2:  strcpy(style, "cross"); break;
      case 5:  strcpy(style, "x"); break;
      case 25: strcpy(style, "box"); break;
      case 27: strcpy(style, "diamond"); break;
      default: strcpy(style, "circle"); 
      }

   return style;
}

//_____________________________________________________________________________
void AstroPoint::WriteRegion(FILE * file, WorldCoor * wcs)
{
// Writes this region and its attributres into a region file.

   AstroRegion::WriteRegion(file, wcs);

   double axisA, axisB;

   pix2wcs(wcs, fImgX, fImgY, &axisA, &axisB);
   fprintf(file, "point (%9.4f, %9.4f) # point = %s", axisA, axisB, 
                 MarkerStyleToString(GetMarkerStyle()) );


   if (!fRegionText.IsNull())
      fprintf(file, " text = {%s}", fRegionText.Data() );

   fprintf(file, "\n");
}




