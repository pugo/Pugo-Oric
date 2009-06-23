/***************************************************************************
 *   Copyright (C) 2009 by Anders Karlsson   *
 *   pugo@pugo.org   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DATATYPES_H
#define DATATYPES_H

typedef unsigned char byte;
typedef unsigned short word;

/* typedef union */
/* { */

/*   unsigned short w;        /\* Word access *\/ */

/*   struct          /\* Byte access... *\/ */
/*   { */
/* #if __BYTE_ORDER == __LITTLE_ENDIAN */
/*     byte l,h;     /\* ...in low-endian architecture *\/ */
/* #else */
/*     byte h,l;     /\* ...in high-endian architecture *\/ */
/* #endif */
/*   } b; */

/* } word; */



#endif // DATATYPES_H
