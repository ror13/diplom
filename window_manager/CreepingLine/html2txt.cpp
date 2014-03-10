/*
 * HTML2TXT
 *
 * Copyright 2000 Matteo Baccan <mbaccan@planetisa.com>
 * www - http://www.infomedia.it/artic/Baccan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA (or visit
 * their web site at http://www.gnu.org/).
 *
 */

#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "html2txt.h"

// LPTSTR CCommandLineParser::FindOneOf(LPCTSTR lpszCmdLine, LPCTSTR Tokens)
// {
// 	BOOL bQuoted = FALSE;
// 	
//     while (lpszCmdLine != NULL && *lpszCmdLine != NULL)
//     {
//         LPTSTR p = (LPTSTR)Tokens;
//         while (p != NULL && *p != NULL)
//         {
//             if (*lpszCmdLine == *p && !bQuoted)
//                 return CharNext(lpszCmdLine);
//             p = CharNext(p);
//         }
// 		
// 		if(lpszCmdLine[0] == _T('\"'))
// 			bQuoted = !bQuoted;
// 		
//         lpszCmdLine = CharNext(lpszCmdLine);
//     }
//     return NULL;
// }

int dfCheckChar(int iLen, LPTSTR Buffer, int iPointer, LPCTSTR Check, int iCheckLen)
{
	BOOL bRes = FALSE;
	if(iLen + 1 >= iCheckLen)
	{
		iPointer--;
		while(*Check != '\0')
		{
			if(Buffer[iPointer] == *Check ||
				(Buffer[iPointer]|32) == *Check)
			{
				iPointer++;
				Check++;
			} else
				break;
		}
		bRes = (*Check == 0 || *Check == 13 || *Check == ';' || *Check == ' ');
	}
		
	return bRes;
}

// convert string
void dfConvert(LPTSTR Buffer)
{
   BOOL bStartTag = FALSE;

   int  iPos, iPointer;
   int  iLen = lstrlen(Buffer);

   iPos=iPointer=0;
   while( iLen-->0 ){
      iPointer++;

      // BlockQuote
      if( dfCheckChar( iLen, Buffer, iPointer, _T("<BLOCKQUOTE>"), 12 ) ){
          Buffer[iPos++]=_T('"');
          iPointer+=11;
          continue;
      }
      if( dfCheckChar( iLen, Buffer, iPointer, _T("</BLOCKQUOTE>"), 13 ) ){
          Buffer[iPos++]=_T('"');
          iPointer+=12;
          continue;
      }

      // LineBreak
      if( dfCheckChar( iLen, Buffer, iPointer, _T("<BR>"), 4 ) ){
          //Buffer[iPos++]=0x0d;
          Buffer[iPos++]=_T(' ');
          iPointer+=3;
          continue;
      }
      if( dfCheckChar( iLen, Buffer, iPointer, _T("</BR>"), 5 ) ){
          //Buffer[iPos++]=0x0d;
          Buffer[iPos++]=_T(' ');
          iPointer+=4;
          continue;
      }

      // Citation
      if( dfCheckChar( iLen, Buffer, iPointer, _T("<CITE>"), 6 ) ){
          Buffer[iPos++]=_T('"');
          iPointer+=5;
          continue;
      }
      if( dfCheckChar( iLen, Buffer, iPointer, _T("</CITE>"), 7 ) ){
          Buffer[iPos++]=_T('"');
          iPointer+=6;
          continue;
      }

      // Tab
      if( dfCheckChar( iLen, Buffer, iPointer, _T("<TD>"), 4 ) ){
          Buffer[iPos++]=_T(' ');
          iPointer+=3;
          continue;
      }
      if( dfCheckChar( iLen, Buffer, iPointer, _T("</TD>"), 5 ) ){
          Buffer[iPos++]=_T(' ');
          iPointer+=4;
          continue;
      }

      // HTML Command Skipper
      if( Buffer[iPointer-1]=='<' ){
         if( Buffer[iPointer]!='\0' ){
            if( Buffer[iPointer]>=_T('a') && Buffer[iPointer]<=_T('z') ) bStartTag=1;
            if( Buffer[iPointer]>=_T('A') && Buffer[iPointer]<=_T('Z') ) bStartTag=1;
            if( Buffer[iPointer]==_T('!')                          ) bStartTag=1;
            if( Buffer[iPointer]==_T('/')                          ) bStartTag=1;
         }
      }
      if( Buffer[iPointer-1]==_T('>') && bStartTag==1 ) {
         bStartTag=0;
         continue;
      }

      if( bStartTag==0 ){
         if( Buffer[iPointer-1]==_T('&') ){
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&lt;")     , 4 ) ){ Buffer[iPos++]=_T('<'); iPointer+=3; continue; } //4
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&gt;")     , 4 ) ){ Buffer[iPos++]=_T('>'); iPointer+=3; continue; } //4
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&amp;")    , 5 ) ){ Buffer[iPos++]=_T('&'); iPointer+=4; continue; } //5
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&quot;")   , 6 ) ){ Buffer[iPos++]=_T('"'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Aacute;") , 8 ) ){ Buffer[iPos++]=_T(' '); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Agrave;") , 8 ) ){ Buffer[iPos++]=_T('…'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Acirc;")  , 7 ) ){ Buffer[iPos++]=_T('ƒ'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Atilde;") , 8 ) ){ Buffer[iPos++]=_T('†'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Aring;")  , 7 ) ){ Buffer[iPos++]=_T(''); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Auml;")   , 6 ) ){ Buffer[iPos++]=_T('„'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&AElig;")  , 7 ) ){ Buffer[iPos++]=_T('’'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ccedil;") , 8 ) ){ Buffer[iPos++]=_T('‡'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Eacute;") , 8 ) ){ Buffer[iPos++]=_T('‚'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Egrave;") , 8 ) ){ Buffer[iPos++]=_T('Š'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ecirc;")  , 7 ) ){ Buffer[iPos++]=_T('ˆ'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Euml;")   , 6 ) ){ Buffer[iPos++]=_T('‰'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Iacute;") , 8 ) ){ Buffer[iPos++]=_T('¡'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Igrave;") , 8 ) ){ Buffer[iPos++]=_T(''); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Icirc;")  , 7 ) ){ Buffer[iPos++]=_T('Œ'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Iuml;")   , 6 ) ){ Buffer[iPos++]=_T('‹'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ETH;")    , 5 ) ){ Buffer[iPos++]=_T('Ñ'); iPointer+=4; continue; } //5
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ntilde;") , 8 ) ){ Buffer[iPos++]=_T('¤'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Oacute;") , 8 ) ){ Buffer[iPos++]=_T('¢'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ograve;") , 8 ) ){ Buffer[iPos++]=_T('•'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ocirc;")  , 7 ) ){ Buffer[iPos++]=_T('“'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Otilde;") , 8 ) ){ Buffer[iPos++]=_T('”'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ouml;")   , 6 ) ){ Buffer[iPos++]=_T('”'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Oslash;") , 8 ) ){ Buffer[iPos++]=_T('0'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Uacute;") , 8 ) ){ Buffer[iPos++]=_T('£'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ugrave;") , 8 ) ){ Buffer[iPos++]=_T('—'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Ucirc;")  , 7 ) ){ Buffer[iPos++]=_T('–'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Uuml;")   , 6 ) ){ Buffer[iPos++]=_T(''); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&Yacute;") , 8 ) ){ Buffer[iPos++]=_T('Y'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&THORN;")  , 7 ) ){ Buffer[iPos++]=_T('è'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&szlig;")  , 7 ) ){ Buffer[iPos++]=_T('á'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&aacute;") , 8 ) ){ Buffer[iPos++]=_T(' '); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&agrave;") , 8 ) ){ Buffer[iPos++]=_T('…'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&acirc;")  , 7 ) ){ Buffer[iPos++]=_T('ƒ'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&atilde;") , 8 ) ){ Buffer[iPos++]=_T('†'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&aring;")  , 7 ) ){ Buffer[iPos++]=_T(''); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&auml;")   , 6 ) ){ Buffer[iPos++]=_T('„'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&aelig;")  , 7 ) ){ Buffer[iPos++]=_T('‘'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ccedil;") , 8 ) ){ Buffer[iPos++]=_T('‡'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&eacute;") , 8 ) ){ Buffer[iPos++]=_T('‚'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&egrave;") , 8 ) ){ Buffer[iPos++]=_T('Š'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ecirc;")  , 7 ) ){ Buffer[iPos++]=_T('ˆ'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&euml;")   , 6 ) ){ Buffer[iPos++]=_T('‰'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&iacute;") , 8 ) ){ Buffer[iPos++]=_T('¡'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&igrave;") , 8 ) ){ Buffer[iPos++]=_T(''); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&icirc;")  , 7 ) ){ Buffer[iPos++]=_T('Œ'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&iuml;")   , 6 ) ){ Buffer[iPos++]=_T('‹'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&eth;")    , 5 ) ){ Buffer[iPos++]=_T('Ñ'); iPointer+=4; continue; } //5
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ntilde;") , 8 ) ){ Buffer[iPos++]=_T('¤'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&oacute;") , 8 ) ){ Buffer[iPos++]=_T('¢'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ograve;") , 8 ) ){ Buffer[iPos++]=_T('•'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ocirc;")  , 7 ) ){ Buffer[iPos++]=_T('“'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&otilde;") , 8 ) ){ Buffer[iPos++]=_T('”'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ouml;")   , 6 ) ){ Buffer[iPos++]=_T('”'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&oslash;") , 8 ) ){ Buffer[iPos++]=_T('0'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&uacute;") , 8 ) ){ Buffer[iPos++]=_T('£'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ugrave;") , 8 ) ){ Buffer[iPos++]=_T('—'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&ucirc;")  , 7 ) ){ Buffer[iPos++]=_T('–'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&uuml;")   , 6 ) ){ Buffer[iPos++]=_T(''); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&yacute;") , 8 ) ){ Buffer[iPos++]=_T('Y'); iPointer+=7; continue; } //8
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&thorn;")  , 7 ) ){ Buffer[iPos++]=_T('è'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&yuml;")   , 6 ) ){ Buffer[iPos++]=_T('˜'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&reg;")    , 5 ) ){ Buffer[iPos++]=_T('(');
                                                                         Buffer[iPos++]=_T('r');
                                                                         Buffer[iPos++]=_T(')'); iPointer+=4; continue; } //5
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&copy;")   , 6 ) ){ Buffer[iPos++]=_T('(');
                                                                         Buffer[iPos++]=_T('c');
                                                                         Buffer[iPos++]=_T(')'); iPointer+=5; continue; } //6
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&trade;")  , 7 ) ){ Buffer[iPos++]=_T('t');
                                                                         Buffer[iPos++]=_T('m'); iPointer+=6; continue; } //7
            if( dfCheckChar( iLen, Buffer, iPointer, _T("&nbsp;")   , 6 ) ){ Buffer[iPos++]=_T('	'); iPointer+=5; continue; } //6

            // &#number
//             if( Buffer[iPointer]==_T('#') ){
//                // May be a Number
//                int nCount = 0;
//                while( iLen-(nCount+1)>0              && // I have char?
//                       Buffer[iPointer+1+nCount]>=_T('0') && // Are number ?
//                       Buffer[iPointer+1+nCount]<=_T('9') ){
//                   nCount++;
//                }
// 
//                // If I have number .. try to convert it
//                if( nCount>0 ){
//                   int nDmm  = 0;
//                   int nChar = 0;
//                   int nMul  = 1;
//                   while( nDmm<nCount ){
//                      nChar += (Buffer[iPointer+nCount-nDmm]-48)*nMul;
//                   printf( _T("%d\n"), Buffer[iPointer+nCount-nDmm]-48 );
//                   printf( _T("%d\n"), nMul );
//                      nMul  *= 10;
//                      nDmm++;
//                   }
//                   if( nChar>0 ){
//                      Buffer[iPos++]=nChar;
//                      iPointer+=nDmm+1;
//                      continue;
//                   }
//                }
//            }
         }

         if(Buffer[iPointer-1] == _T('\r') && Buffer[iPointer] == _T('\n'))
		 {
             Buffer[iPos++]=_T(' ');
			 continue;
         }

         if(Buffer[iPointer-1] == _T('\n'))
		 {
             Buffer[iPos++]=_T(' ');
			 continue;
         }

         if(Buffer[iPointer-1] == _T('\t'))
		 {
			 continue;
         }

         Buffer[iPos++]=Buffer[iPointer-1];
      }
   }
   Buffer[iPos++]=_T('\0');
}

