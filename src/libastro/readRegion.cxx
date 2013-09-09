// ////////////////////////////////////////////////////////////////////////////
//
//  File:      readRegion.cxx
//
//  Version:   1.0
//
//  Author:    Reiner Rohlfs (GADC)
//
//  History:   1.0   26.04.04  first released version
//
// ////////////////////////////////////////////////////////////////////////////
#include <ctype.h>

#include "astroImage.h"
#include "astroRegion.h"
#include "wcs.h"

//_____________________________________________________________________________

RegionProperties::RegionProperties( const RegionProperties & prop)
{
   color         = prop.color;      
   text          = prop.text;       
   strcpy(arrow, prop.arrow);
   textAngle     = prop.textAngle;  
   markerStyle   = prop.markerStyle;
}

//_____________________________________________________________________________
//_____________________________________________________________________________

static void ReadRegionProperties( RegionProperties & properties)
{
   char * type;
   while ((type = strtok(NULL, ")#= ")) != NULL)
      {
      if (strcmp("color", type) == 0)
         {
         // the color property
         char * color = strtok(NULL, "= \n");
         if (color == NULL)  return;
              if (strcmp(color, "black") == 0)   properties.color = 1;
         else if (strcmp(color, "red") == 0)     properties.color = 2;
         else if (strcmp(color, "green") == 0)   properties.color = 3;
         else if (strcmp(color, "blue") == 0)    properties.color = 4;
         else if (strcmp(color, "yellow") == 0)  properties.color = 5;
         else if (strcmp(color, "magenta") == 0) properties.color = 6;
         else if (strcmp(color, "cyan") == 0)    properties.color = 7;
         else if (strcmp(color, "white") == 0)   properties.color = 10;
         }
      
      else if (strcmp("text", type) == 0)
         {
         // the text property
         char * text;
         if (*(type + 5) == '{')
            text = strtok(NULL, "{}");
         else
            {
            strtok(NULL, "{");
            text = strtok(NULL, "}");
            }
         if (text == NULL) return;
         properties.text = text;
         }


      else if (strcmp("textangle", type) == 0)
         {
         // the angle of the text
         char * angle = strtok(NULL, "= \n");
         if (angle == NULL) return;
         sscanf(angle, "%lf", &properties.textAngle);
         }

      else if (strcmp("line", type) == 0)
         {
         // the arrows of a line
         char * arrow = strtok(NULL, "= ");
         if (arrow == NULL) return;
         if (arrow[0] == '1')  strcpy(properties.arrow, "<");

         arrow = strtok(NULL, " \n");
         if (arrow == NULL) return;
         if (arrow[0] == '1')  strcat(properties.arrow, ">");
         }

      else if (strcmp("point", type) == 0)
         {
         // the marker style (style of the point region
         char * style = strtok(NULL, "= \n");
         if (style == NULL) return;
              if (strcmp(style, "cross") == 0)   properties.markerStyle = 2;
         else if (strcmp(style, "x") == 0)       properties.markerStyle = 5;
         else if (strcmp(style, "box") == 0)     properties.markerStyle = 25;
         else if (strcmp(style, "diamond") == 0) properties.markerStyle = 27;
         else if (strcmp(style, "circle") == 0)  properties.markerStyle = 24;
         }
      }

} 

//_____________________________________________________________________________

static Bool_t Region2ImgPixel(WorldCoor * wcs, const char * currentSystem,
                             char * rg1, char * rg2, 
                             double * imgX, double * imgY)
{
   if (rg1 == NULL || rg2 == NULL)
      return kFALSE;

   double realRg1, realRg2;
   sscanf(rg1, "%lf", &realRg1);
   sscanf(rg2, "%lf", &realRg2);

   if (strcmp(currentSystem, "IMAGE") == 0)
      {
      *imgX = realRg1;
      *imgY = realRg2;
      }
   else
      {
      int offscl;
      wcs2pix(wcs, realRg1, realRg2, imgX, imgY, &offscl);
      }

   return kTRUE;
}

//_____________________________________________________________________________

static Bool_t Region2ImgSize(WorldCoor * wcs, const char * currentSystem,
                             double imgCenterX, double imgCenterY,
                             char * rg, double * imgS)
{
   if (rg == NULL)
      return kFALSE;

   double realRg;
   sscanf(rg, "%lf", &realRg);

   char dimension = rg[strlen(rg) -1];

   if (dimension == '\"' || dimension == '\'' || 
       dimension == 'd'  || dimension == 'r'     )
      {
      double ra0, dec0, ra1, dec1;
      pix2wcs(wcs, imgCenterX, imgCenterY, &ra0, &dec0);
      pix2wcs(wcs, imgCenterX, imgCenterY + 1, &ra1, &dec1);
      double pixelSize = wcsdist(ra0, dec0, ra1, dec1);
      if (pixelSize > 0 && pixelSize <= 360.)
         {
         if (dimension == '\"')
            *imgS = realRg / 3600. / pixelSize;
         else if (dimension == '\'')
            *imgS = realRg / 60. / pixelSize;
         else if (dimension == 'd')
            *imgS = realRg / pixelSize;
         else if (dimension == 'r')
            *imgS = realRg / 3.1416 * 180. / pixelSize;
         }
      else
         *imgS = 10;
      }

   else if (strcmp(currentSystem, "IMAGE") == 0 ||
            dimension == 'i')
      {
      *imgS = realRg;
      }

   else
      {
      *imgS = 10;
      }

   return kTRUE;
}

//_____________________________________________________________________________

void AstroImage::Read1Region(char * line, char * currentSystem,
                             RegionProperties & globalProperties)
{
   char * type = strtok(line, " (\t");

   if (type == NULL) return;

   char * tmp = type;
   while (*tmp)
      {
      *tmp = toupper(*tmp);
      tmp++;
      }

   if (strcmp(type, "IMAGE") == 0    ||
       strcmp(type, "LINEAR") == 0   ||
       strcmp(type, "FK5") == 0      ||
       strcmp(type, "FK4") == 0      ||
       strcmp(type, "B1950") == 0    ||
       strcmp(type, "J2000") == 0    ||
       strcmp(type, "GALACTIC") == 0 ||
       strcmp(type, "ECLIPTIC") == 0 ||
       strcmp(type, "ICRS") == 0     ||
       strcmp(type, "PHYSICAL") == 0    )
      {
      // a new coordiante system
      strcpy(currentSystem, type);
      // nothing else to do
      }

   else if (strcmp(type, "GLOBAL") == 0  )
      ReadRegionProperties(globalProperties);
      
   else if (strcmp(type, "LINE") == 0  )
      new AstroLine(fWcs, currentSystem, globalProperties);

   else if (strcmp(type, "TEXT") == 0 )
      new AstroText(fWcs, currentSystem, globalProperties);

   else if (strcmp(type, "CIRCLE") == 0 )
      new AstroCircle(fWcs, currentSystem, globalProperties);

   else if (strcmp(type, "ELLIPSE") == 0 )
      new AstroEllipse(fWcs, currentSystem, globalProperties);

   else if (strcmp(type, "POINT") == 0 )
      new AstroPoint(fWcs, currentSystem, globalProperties);
}

//_____________________________________________________________________________

AstroLine::AstroLine(WorldCoor * wcs, char * currentSystem, 
                     RegionProperties & globalProperties)
   : TArrow(0.2, 0.5, 0.8, 0.5)
{

   char prevSystem[20];
   strcpy(prevSystem, getwcsin(wcs) );
   wcsininit(wcs, currentSystem);

   // the four coordiantes:
   double coord[4];
   char * tok1 = strtok(NULL, ",( ");
   char * tok2 = strtok(NULL, ", ");
   char * tok3 = strtok(NULL, ", ");
   char * tok4 = strtok(NULL, ",) #");
   if ( !Region2ImgPixel(wcs, currentSystem, tok1, tok2,
                         coord, coord+1)  ||
        !Region2ImgPixel(wcs, currentSystem, tok3, tok4,
                         coord+2, coord+3) )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   wcsininit(wcs, prevSystem);


   fImgX = coord[0];
   fImgY = coord[1];
   fImgX2 = coord[2];
   fImgY2 = coord[3];

   // read properties
   RegionProperties properties(globalProperties);
   ReadRegionProperties(properties);
   SetLineColor(properties.color);
   fRegionText = properties.text;

   SetOption(properties.arrow);

   // some default values
   SetLineWidth(2);
   SetArrowSize(0.02);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

   Draw();
}

//_____________________________________________________________________________

AstroText::AstroText(WorldCoor * wcs, char * currentSystem, 
                     RegionProperties & globalProperties)
{

   char prevSystem[20];
   strcpy(prevSystem, getwcsin(wcs) );
   wcsininit(wcs, currentSystem);

   // the coordiantes:
   double coord[2];
   char * tok1 = strtok(NULL, ",( ");
   char * tok2 = strtok(NULL, ",) #");
   if ( !Region2ImgPixel(wcs, currentSystem, tok1, tok2,
                         coord, coord+1) )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   wcsininit(wcs, prevSystem);

   fImgX = coord[0];
   fImgY = coord[1];

   // read properties
   RegionProperties properties(globalProperties);
   ReadRegionProperties(properties);

   if (properties.text.IsNull())
      {
      delete this;
      return;
      }

   SetTextColor(properties.color);

   // some default values
   SetTitle(properties.text.Data());
   SetTextAngle(properties.textAngle);

   Draw();
}

//_____________________________________________________________________________

AstroCircle::AstroCircle(WorldCoor * wcs, char * currentSystem, 
                        RegionProperties & globalProperties)
 : TEllipse(0, 0, 0.10)
{
   char prevSystem[20];
   strcpy(prevSystem, getwcsin(wcs) );
   wcsininit(wcs, currentSystem);

   // the coordiantes:
   double coord[2];
   char * tok1 = strtok(NULL, ",( ");
   char * tok2 = strtok(NULL, ", ");
   if ( !Region2ImgPixel(wcs, currentSystem, tok1, tok2,
                         coord, coord+1) )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   fImgX = coord[0];
   fImgY = coord[1];

   if ( !Region2ImgSize(wcs, currentSystem, fImgX, fImgY,
                        strtok(NULL, ", )#"), coord) )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   fImgR = coord[0];   
   
   wcsininit(wcs, prevSystem);

   // read properties
   RegionProperties properties(globalProperties);
   ReadRegionProperties(properties);
   SetLineColor(properties.color);
   fRegionText = properties.text;

   // some default values
   SetLineWidth(2);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

   Draw();
}

//_____________________________________________________________________________

AstroEllipse::AstroEllipse(WorldCoor * wcs, char * currentSystem, 
                           RegionProperties & globalProperties)
 : TEllipse(0, 0, 0.10)
{
   char prevSystem[20];
   strcpy(prevSystem, getwcsin(wcs) );
   wcsininit(wcs, currentSystem);

   // the coordiantes:
   double coord[2];
   char * tok1 = strtok(NULL, ",( ");
   char * tok2 = strtok(NULL, ", ");
   if ( !Region2ImgPixel(wcs, currentSystem, tok1, tok2,
                         coord, coord+1) )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   fImgX = coord[0];
   fImgY = coord[1];

   // the radius R1 and R2
   if ( !Region2ImgSize(wcs, currentSystem, fImgX, fImgY,
                        strtok(NULL, ", "), coord) ||
        !Region2ImgSize(wcs, currentSystem, fImgX, fImgY,
                        strtok(NULL, ", "), coord + 1)                        
                         )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   fImgR1 = coord[0];   
   fImgR2 = coord[1];   
   
   wcsininit(wcs, prevSystem);

   // the angle
   char * angle = strtok(NULL, ", )#");
   if (angle == NULL)
      {
      delete this;
      return;
      }
   double theta;
   sscanf(angle, "%lf", &theta);
   SetTheta(theta);


   // read properties
   RegionProperties properties(globalProperties);
   ReadRegionProperties(properties);
   SetLineColor(properties.color);
   fRegionText = properties.text;

   // some default values
   SetLineWidth(2);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

   Draw();
}

//_____________________________________________________________________________

AstroPoint::AstroPoint(WorldCoor * wcs, char * currentSystem, 
                       RegionProperties & globalProperties)
   : TMarker(0.2, 0.5, 24)
{

   char prevSystem[20];
   strcpy(prevSystem, getwcsin(wcs) );
   wcsininit(wcs, currentSystem);

   // the coordiantes:
   double coord[2];
   char * tok1 = strtok(NULL, ",( ");
   char * tok2 = strtok(NULL, ",) #");
   if ( !Region2ImgPixel(wcs, currentSystem, tok1, tok2,
                         coord, coord+1) )
      {
      wcsininit(wcs, prevSystem);
      delete this;
      return;
      }

   wcsininit(wcs, prevSystem);

   fImgX = coord[0];
   fImgY = coord[1];

   // read properties
   RegionProperties properties(globalProperties);
   ReadRegionProperties(properties);

   fRegionText = properties.text;
   SetMarkerColor(properties.color);
   SetMarkerStyle(properties.markerStyle);

   // some default values
   SetMarkerSize(1.5);

   SetTextAlign(21);
   SetTextFont(63);
   SetTextSize(17);

   Draw();
}

