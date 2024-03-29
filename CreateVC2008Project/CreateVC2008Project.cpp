/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "CreateVC2008Project.h"

bool CreateVC2008Project( const char * pszFolder, const char * pszSolutionName )
{
	CSolutionInfo clsSI;

	if( clsSI.SetFolder( pszFolder, pszSolutionName ) == false )
	{
		printf( "clsPI.SetFolder(%s) error\n", pszFolder );
		return false;
	}

	if( CreateSolution( clsSI ) == false )
	{
		printf( "CreateSolution(%s) error\n", pszFolder );
		return false;
	}

	PROJECT_INFO_LIST::iterator itPIL;

	for( itPIL = clsSI.m_clsProjectInfoList.begin(); itPIL != clsSI.m_clsProjectInfoList.end(); ++itPIL )
	{
		if( CreateProject( clsSI, *itPIL ) == false )
		{
			printf( "CreateSolution(%s) error\n", pszFolder );
			return false;
		}
	}

	return true;
}

int main( int argc, char * argv[] )
{
	if( argc != 3 )
	{
		printf( "[Usage] %s {folder} {solution name}\n", argv[0] );
		return 0;
	}

	CreateVC2008Project( argv[1], argv[2] );

	return 0;
}
