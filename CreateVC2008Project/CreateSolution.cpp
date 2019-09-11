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

bool CreateSolution( CSolutionInfo & clsSI )
{
	std::string strFileName = clsSI.m_strFolder;
	strFileName.append( "\\" );
	strFileName.append( clsSI.m_strSolutionName );
	strFileName.append( ".sln" );

	FILE * fd = fopen( strFileName.c_str(), "wb" );
	if( fd == NULL )
	{
		printf( "%s fopen(%s) error\n", __FUNCTION__, strFileName.c_str() );
		return false;
	}

	unsigned char szBOM[3] = { 0xEF, 0xBB, 0xBF };

	fwrite( szBOM, 1, 3, fd );

	fprintf( fd, "\r\n" );
	fprintf( fd, "Microsoft Visual Studio Solution File, Format Version 10.00\r\n" );
	fprintf( fd, "# Visual Studio 2008\r\n" );

	PROJECT_INFO_LIST::iterator itPIL;

	for( itPIL = clsSI.m_clsProjectInfoList.begin(); itPIL != clsSI.m_clsProjectInfoList.end(); ++itPIL )
	{
		fprintf( fd, "Project(\"%s\") = \"%s\", \"%s.vcproj\", \"%s\"\r\n"
			, clsSI.m_strSolutionGUID.c_str(), clsSI.m_strSolutionName.c_str(), itPIL->m_strProjectName.c_str(), itPIL->m_strProjectGUID.c_str() );
		fprintf( fd, "EndProject\r\n" );
	}	
	
	fprintf( fd, "Global\r\n" );
	fprintf( fd, "	GlobalSection(SolutionConfigurationPlatforms) = preSolution\r\n" );
	fprintf( fd, "		Debug|Win32 = Debug|Win32\r\n" );
	fprintf( fd, "		Release|Win32 = Release|Win32\r\n" );
	fprintf( fd, "	EndGlobalSection\r\n" );
	fprintf( fd, "	GlobalSection(ProjectConfigurationPlatforms) = postSolution\r\n" );

	for( itPIL = clsSI.m_clsProjectInfoList.begin(); itPIL != clsSI.m_clsProjectInfoList.end(); ++itPIL )
	{
		fprintf( fd, "		%s.Debug|Win32.ActiveCfg = Debug|Win32\r\n", itPIL->m_strProjectGUID.c_str() );
		fprintf( fd, "		%s.Debug|Win32.Build.0 = Debug|Win32\r\n", itPIL->m_strProjectGUID.c_str() );
		fprintf( fd, "		%s.Release|Win32.ActiveCfg = Release|Win32\r\n", itPIL->m_strProjectGUID.c_str() );
		fprintf( fd, "		%s.Release|Win32.Build.0 = Release|Win32\r\n", itPIL->m_strProjectGUID.c_str() );
	}

	fprintf( fd, "	EndGlobalSection\r\n" );
	fprintf( fd, "	GlobalSection(SolutionProperties) = preSolution\r\n" );
	fprintf( fd, "		HideSolutionNode = FALSE\r\n" );
	fprintf( fd, "	EndGlobalSection\r\n" );
	fprintf( fd, "EndGlobal\r\n" );

	fclose( fd );

	return true;
}


