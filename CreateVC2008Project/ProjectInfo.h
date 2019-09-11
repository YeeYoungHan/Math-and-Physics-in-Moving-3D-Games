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

#ifndef _PROJECT_INFO_H_
#define _PROJECT_INFO_H_

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <list>

class CProjectInfo
{
public:
	std::string m_strFileName;
	std::string m_strProjectName;
	std::string m_strProjectGUID;
};

typedef std::list< CProjectInfo > PROJECT_INFO_LIST;

class CSolutionInfo
{
public:
	bool SetFolder( const char * pszFolder, const char * pszSolutionName );

	std::string m_strFolder;
	std::string m_strSolutionName;
	std::string m_strSolutionGUID;

	PROJECT_INFO_LIST m_clsProjectInfoList;
};

#endif
