/*
=================================================================
Copyright (C) 2005-2006 Torben Koenke

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA  02110-1301, USA.
=================================================================
*/

/*
# myPS2
# Author: tk
# Date: 17-7-2005
#
# File: PS2 Smooth Bitmap Resizing.
#		Based on 'Accelerated Smooth Bitmap Resizing'
#		article written by Ivaylo Byalkov.
#
#
*/

#include <imgscale.h>

//
// ScaleBitmap
//
// Resizes bitmap to new dimensions. Notice that both wNewWidth 
// and wNewHeight should be greater or lesser than the dimensions 
// of the original bitmap.
//
// Image data must be RGB 24 bits per pixel!
// RGBA 32 bits per pixel is currently not supported.
//
// The function dynamically allocates memory for the new bitmap
// data and ensures it is aligned on a 128 bit boundary.
//

int ScaleBitmap( u8 *pInBuff, u16 wWidth, u16 wHeight, u8 **pOutBuff, u16 wNewWidth, u16 wNewHeight )
{
	int nRet;

	// check for valid size
	if( wWidth > wNewWidth && wHeight < wNewHeight )
		return 0;

	if( wHeight > wNewHeight && wWidth < wNewWidth )
		return 0;

	// allocate memory
	*pOutBuff = (u8*) memalign( 128, ((3 * wNewWidth + 3) & ~3) * wNewHeight );

	if(!*pOutBuff) {
#ifdef _DEBUG
		printf("ScaleBitmap : failed to allocated memory.\n");
#endif
		return 0;
	}

	if( wWidth >= wNewWidth && wHeight >= wNewHeight )
		nRet = ShrinkData( pInBuff, wWidth, wHeight, *pOutBuff, wNewWidth, wNewHeight );
	else
		nRet = EnlargeData( pInBuff, wWidth, wHeight, *pOutBuff, wNewWidth, wNewHeight );

	return nRet;
}


int ShrinkData( u8 *pInBuff, u16 wWidth, u16 wHeight, u8 *pOutBuff, u16 wNewWidth, u16 wNewHeight )
{
	u8  *pLine		= pInBuff, *pPix;
	u8	*pOutLine	= pOutBuff;
	u32 dwInLn		= (3 * wWidth + 3) & ~3;
	u32 dwOutLn		= (3 * wNewWidth + 3) & ~3;

	int x, y, i, ii;
	int	bCrossRow, bCrossCol;
	int	*pRowCoeff	= CreateCoeffInt( wWidth, wNewWidth, 1 );
	int	*pColCoeff	= CreateCoeffInt( wHeight, wNewHeight, 1 );

	int	*pXCoeff, *pYCoeff = pColCoeff;
	u32 dwBuffLn	= 3 * wNewWidth * sizeof(u32);
	u32 *pdwBuff	= (u32*) malloc( 6 * wNewWidth * sizeof(u32) );
	u32 *pdwCurrLn	= pdwBuff; 	
	u32 *pdwNextLn	= pdwBuff + 3 * wNewWidth;
	u32 *pdwCurrPix;
	u32 dwTmp, *pdwNextPix;

	memset( pdwBuff, 0, 2 * dwBuffLn );

	y = 0;
	while( y < wNewHeight )
	{
		pPix	= pLine;
		pLine	+= dwInLn;

		pdwCurrPix = pdwCurrLn;
		pdwNextPix = pdwNextLn;

		x = 0;
		pXCoeff = pRowCoeff;
		bCrossRow = pYCoeff[1] > 0;
		while(x < wNewWidth)
		{
			dwTmp = *pXCoeff * *pYCoeff;
			for(i = 0; i < 3; i++)
				pdwCurrPix[i] += dwTmp * pPix[i];
			
			bCrossCol = pXCoeff[1] > 0;
			if(bCrossCol)
			{
				dwTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0, ii = 3; i < 3; i++, ii++)
					pdwCurrPix[ii] += dwTmp * pPix[i];
			}
			
			if(bCrossRow)
			{
				dwTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					pdwNextPix[i] += dwTmp * pPix[i];
				
				if(bCrossCol)
				{
					dwTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0, ii = 3; i < 3; i++, ii++)
						pdwNextPix[ii] += dwTmp * pPix[i];
				}
			}

			if(pXCoeff[1])
			{
				x++;
				pdwCurrPix += 3;
				pdwNextPix += 3;
			}
			
			pXCoeff += 2;
			pPix += 3;
		}
		
		if(pYCoeff[1])
		{
			// set result line
			pdwCurrPix = pdwCurrLn;
			pPix = pOutLine;
			
			for(i = 3 * wNewWidth; i > 0; i--, pdwCurrPix++, pPix++)
				*pPix = ((u8*)pdwCurrPix)[3];

			// prepare line buffers
			pdwCurrPix = pdwNextLn;
			pdwNextLn = pdwCurrLn;
			pdwCurrLn = pdwCurrPix;
			
			memset( pdwNextLn, 0, dwBuffLn );
			
			y++;
			pOutLine += dwOutLn;
		}
		
		pYCoeff += 2;
	}

	free( pRowCoeff );
	free( pColCoeff );
	free( pdwBuff );

	return 1;
}

int EnlargeData( u8 *pInBuff, u16 wWidth, u16 wHeight, u8 *pOutBuff, u16 wNewWidth, u16 wNewHeight )
{
	u8	*pLine		= pInBuff,
		*pPix		= pLine,
		*pPixOld,
		*pUpPix,
		*pUpPixOld;
	u8  *pOutLine	= pOutBuff, *pOutPix;
	u32 dwInLn		= (3 * wWidth + 3) & ~3;
	u32 dwOutLn		= (3 * wNewWidth + 3) & ~3;

	int	x, y, i;
	int	bCrossRow, bCrossCol;
	
	int	*pRowCoeff = CreateCoeffInt( wNewWidth, wWidth, 0 );
	int	*pColCoeff = CreateCoeffInt( wNewHeight, wHeight, 0 );
	int	*pXCoeff, *pYCoeff = pColCoeff;
	
	u32 dwTmp, dwPtTmp[3];
	
	y = 0;
	while( y < wHeight )
	{
		bCrossRow	= pYCoeff[1] > 0;
		x			= 0;
		pXCoeff		= pRowCoeff;
		pOutPix		= pOutLine;
		pOutLine	+= dwOutLn;
		pUpPix		= pLine;
		
		if(pYCoeff[1])
		{
			y++;
			pLine += dwInLn;
			pPix = pLine;
		}
		
		while( x < wWidth )
		{
			bCrossCol	= pXCoeff[1] > 0;
			pUpPixOld	= pUpPix;
			pPixOld		= pPix;
			
			if(pXCoeff[1])
			{
				x++;
				pUpPix += 3;
				pPix += 3;
			}
			
			dwTmp = *pXCoeff * *pYCoeff;
			
			for(i = 0; i < 3; i++)
				dwPtTmp[i] = dwTmp * pUpPixOld[i];

			if(bCrossCol)
			{
				dwTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0; i < 3; i++)
					dwPtTmp[i] += dwTmp * pUpPix[i];
			}

			if(bCrossRow)
			{
				dwTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					dwPtTmp[i] += dwTmp * pPixOld[i];
				
				if(bCrossCol)
				{
					dwTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0; i < 3; i++)
						dwPtTmp[i] += dwTmp * pPix[i];
				}
			}
			
			for(i = 0; i < 3; i++, pOutPix++)
				*pOutPix = ((u8*)(dwPtTmp + i))[3];
			
			pXCoeff += 2;
		}
		
		pYCoeff += 2;
	}
	
	free( pRowCoeff );
	free( pColCoeff );

	return 1;
}

int *CreateCoeffInt( int nLen, int nNewLen, int bShrink )
{
	int nSum	= 0;
	int nSum2	= 0;
	int *pRes	= (int*) malloc( 2 * nLen * sizeof(int) );
	int *pCoeff = pRes;
	int nNorm	= (bShrink) ? (nNewLen << 12) / nLen : 0x1000;
	int nDenom	= (bShrink) ? nLen : nNewLen;
	int i;

	memset( pRes, 0, 2 * nLen * sizeof(int) );

	for( i = 0; i < nLen; i++, pCoeff += 2 )
	{
		nSum2 = nSum + nNewLen;
		if(nSum2 > nLen) {
			*pCoeff = ((nLen - nSum) << 12) / nDenom;
			pCoeff[1] = ((nSum2 - nLen) << 12) / nDenom;
			nSum2 -= nLen;
		}
		else {
			*pCoeff = nNorm;
			if(nSum2 == nLen) {
				pCoeff[1] = -1;
				nSum2 = 0;
			}
		}
		nSum = nSum2;
	}
	return pRes;
}
