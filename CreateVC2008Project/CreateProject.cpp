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

bool CreateProject( CSolutionInfo & clsSI, CProjectInfo & clsPI )
{
	std::string strFileName = clsSI.m_strFolder;
	strFileName.append( "\\" );
	strFileName.append( clsPI.m_strProjectName );
	strFileName.append( ".vcproj" );

	FILE * fd = fopen( strFileName.c_str(), "w" );
	if( fd == NULL )
	{
		printf( "%s fopen(%s) error\n", __FUNCTION__, strFileName.c_str() );
		return false;
	}

	fprintf( fd, "<?xml version=\"1.0\" encoding=\"ks_c_5601-1987\"?>\r\n" );
	fprintf( fd, "<VisualStudioProject\r\n" );
	fprintf( fd, "	ProjectType=\"Visual C++\"\r\n" );
	fprintf( fd, "	Version=\"9.00\"\r\n" );
	fprintf( fd, "	Name=\"%s\"\r\n", clsPI.m_strProjectName.c_str() );
	fprintf( fd, "	ProjectGUID=\"%s\"\r\n", clsPI.m_strProjectGUID.c_str() );
	fprintf( fd, "	RootNamespace=\"%s\"\r\n", clsPI.m_strProjectName.c_str() );
	fprintf( fd, "	Keyword=\"Win32Proj\"\r\n" );
	fprintf( fd, "	TargetFrameworkVersion=\"196613\"\r\n" );
	fprintf( fd, "	>\r\n" );
	fprintf( fd, "	<Platforms>\r\n" );
	fprintf( fd, "		<Platform\r\n" );
	fprintf( fd, "			Name=\"Win32\"\r\n" );
	fprintf( fd, "		/>\r\n" );
	fprintf( fd, "	</Platforms>\r\n" );
	fprintf( fd, "	<ToolFiles>\r\n" );
	fprintf( fd, "	</ToolFiles>\r\n" );
	fprintf( fd, "	<Configurations>\r\n" );
	fprintf( fd, "		<Configuration\r\n" );
	fprintf( fd, "			Name=\"Debug|Win32\"\r\n" );
	fprintf( fd, "			OutputDirectory=\"$(SolutionDir)$(ConfigurationName)\"\r\n" );
	fprintf( fd, "			IntermediateDirectory=\"$(ConfigurationName)\"\r\n" );
	fprintf( fd, "			ConfigurationType=\"1\"\r\n" );
	fprintf( fd, "			CharacterSet=\"1\"\r\n" );
	fprintf( fd, "			>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCPreBuildEventTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCCustomBuildTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCXMLDataGeneratorTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCWebServiceProxyGeneratorTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCMIDLTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCCLCompilerTool\"\r\n" );
	fprintf( fd, "				Optimization=\"0\"\r\n" );
	fprintf( fd, "				AdditionalIncludeDirectories=\"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include;../../Common\"\r\n" );
	fprintf( fd, "				PreprocessorDefinitions=\"WIN32;_DEBUG;_WINDOWS\"\r\n" );
	fprintf( fd, "				MinimalRebuild=\"true\"\r\n" );
	fprintf( fd, "				BasicRuntimeChecks=\"3\"\r\n" );
	fprintf( fd, "				RuntimeLibrary=\"3\"\r\n" );
	fprintf( fd, "				UsePrecompiledHeader=\"0\"\r\n" );
	fprintf( fd, "				WarningLevel=\"3\"\r\n" );
	fprintf( fd, "				DebugInformationFormat=\"4\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCManagedResourceCompilerTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCResourceCompilerTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCPreLinkEventTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCLinkerTool\"\r\n" );
	fprintf( fd, "				AdditionalDependencies=\"d3d11.lib d3dx11d.lib D3DCompiler.lib dxerr.lib dxgi.lib dxguid.lib\"\r\n" );
	fprintf( fd, "				LinkIncremental=\"2\"\r\n" );
	fprintf( fd, "				AdditionalLibraryDirectories=\"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86;../../Effects11/Debug;../../Common/Debug\"\r\n" );
	fprintf( fd, "				GenerateDebugInformation=\"true\"\r\n" );
	fprintf( fd, "				SubSystem=\"2\"\r\n" );
	fprintf( fd, "				TargetMachine=\"1\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCALinkTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCManifestTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCXDCMakeTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCBscMakeTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCFxCopTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCAppVerifierTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCPostBuildEventTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "		</Configuration>\r\n" );
	fprintf( fd, "		<Configuration\r\n" );
	fprintf( fd, "			Name=\"Release|Win32\"\r\n" );
	fprintf( fd, "			OutputDirectory=\"$(SolutionDir)$(ConfigurationName)\"\r\n" );
	fprintf( fd, "			IntermediateDirectory=\"$(ConfigurationName)\"\r\n" );
	fprintf( fd, "			ConfigurationType=\"1\"\r\n" );
	fprintf( fd, "			CharacterSet=\"1\"\r\n" );
	fprintf( fd, "			WholeProgramOptimization=\"1\"\r\n" );
	fprintf( fd, "			>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCPreBuildEventTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCCustomBuildTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCXMLDataGeneratorTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCWebServiceProxyGeneratorTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCMIDLTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCCLCompilerTool\"\r\n" );
	fprintf( fd, "				Optimization=\"2\"\r\n" );
	fprintf( fd, "				EnableIntrinsicFunctions=\"true\"\r\n" );
	fprintf( fd, "				AdditionalIncludeDirectories=\"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include;../../Common\"\r\n" );
	fprintf( fd, "				PreprocessorDefinitions=\"WIN32;NDEBUG;_WINDOWS\"\r\n" );
	fprintf( fd, "				RuntimeLibrary=\"2\"\r\n" );
	fprintf( fd, "				EnableFunctionLevelLinking=\"true\"\r\n" );
	fprintf( fd, "				UsePrecompiledHeader=\"0\"\r\n" );
	fprintf( fd, "				WarningLevel=\"3\"\r\n" );
	fprintf( fd, "				DebugInformationFormat=\"3\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCManagedResourceCompilerTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCResourceCompilerTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCPreLinkEventTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCLinkerTool\"\r\n" );
	fprintf( fd, "				AdditionalDependencies=\"d3d11.lib d3dx11.lib D3DCompiler.lib dxerr.lib dxgi.lib dxguid.lib\"\r\n" );
	fprintf( fd, "				LinkIncremental=\"1\"\r\n" );
	fprintf( fd, "				AdditionalLibraryDirectories=\"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86;../../Effects11/Release;../../Common/Release\"\r\n" );
	fprintf( fd, "				GenerateDebugInformation=\"true\"\r\n" );
	fprintf( fd, "				SubSystem=\"2\"\r\n" );
	fprintf( fd, "				OptimizeReferences=\"2\"\r\n" );
	fprintf( fd, "				EnableCOMDATFolding=\"2\"\r\n" );
	fprintf( fd, "				TargetMachine=\"1\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCALinkTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCManifestTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCXDCMakeTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCBscMakeTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCFxCopTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCAppVerifierTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "			<Tool\r\n" );
	fprintf( fd, "				Name=\"VCPostBuildEventTool\"\r\n" );
	fprintf( fd, "			/>\r\n" );
	fprintf( fd, "		</Configuration>\r\n" );
	fprintf( fd, "	</Configurations>\r\n" );
	fprintf( fd, "	<References>\r\n" );
	fprintf( fd, "	</References>\r\n" );
	fprintf( fd, "	<Files>\r\n" );
	fprintf( fd, "		<Filter\r\n" );
	fprintf( fd, "			Name=\"�ҽ� ����\"\r\n" );
	fprintf( fd, "			Filter=\"cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx\"\r\n" );
	fprintf( fd, "			UniqueIdentifier=\"{4FC737F1-C7A5-4376-A066-2A32D752A2FF}\"\r\n" );
	fprintf( fd, "			>\r\n" );

	fprintf( fd, "			<File\r\n" );
	fprintf( fd, "				RelativePath=\".\\%s\"\r\n", clsPI.m_strFileName.c_str() );
	fprintf( fd, "				>\r\n" );
	fprintf( fd, "			</File>\r\n" );

	/*
	for( itSL = clsPI.m_clsFxFileList.begin(); itSL != clsPI.m_clsFxFileList.end(); ++itSL )
	{
		fprintf( fd, "			<File\r\n" );
		fprintf( fd, "				RelativePath=\".\\FX\\%s\"\r\n", itSL->c_str() );
		fprintf( fd, "				>\r\n" );
		fprintf( fd, "				<FileConfiguration\r\n" );
		fprintf( fd, "					Name=\"Debug|Win32\"\r\n" );
		fprintf( fd, "					>\r\n" );
		fprintf( fd, "					<Tool\r\n" );
		fprintf( fd, "						Name=\"VCCustomBuildTool\"\r\n" );
		fprintf( fd, "						CommandLine=\"&quot;C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Utilities\\bin\\x86\\fxc.exe&quot; /Fc /Od /Zi /T fx_5_0 /Fo &quot;$(InputDir)\\$(InputName).fxo&quot; &quot;$(InputDir)\\$(InputFileName)&quot;\"\r\n" );
		fprintf( fd, "						Outputs=\"$(InputDir)\\$(InputName).fxo\"\r\n" );
		fprintf( fd, "					/>\r\n" );
		fprintf( fd, "				</FileConfiguration>\r\n" );
		fprintf( fd, "				<FileConfiguration\r\n" );
		fprintf( fd, "					Name=\"Release|Win32\"\r\n" );
		fprintf( fd, "					>\r\n" );
		fprintf( fd, "					<Tool\r\n" );
		fprintf( fd, "						Name=\"VCCustomBuildTool\"\r\n" );
		fprintf( fd, "						CommandLine=\"&quot;C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Utilities\\bin\\x86\\fxc.exe&quot; /Fc /Od /Zi /T fx_5_0 /Fo &quot;$(InputDir)\\$(InputName).fxo&quot; &quot;$(InputDir)\\$(InputFileName)&quot;\"\r\n" );
		fprintf( fd, "						Outputs=\"$(InputDir)\\$(InputName).fxo\"\r\n" );
		fprintf( fd, "					/>\r\n" );
		fprintf( fd, "				</FileConfiguration>\r\n" );
		fprintf( fd, "			</File>\r\n" );
	}
	*/

	fprintf( fd, "		</Filter>\r\n" );
	fprintf( fd, "	</Files>\r\n" );
	fprintf( fd, "	<Globals>\r\n" );
	fprintf( fd, "	</Globals>\r\n" );
	fprintf( fd, "</VisualStudioProject>\r\n" );

	fclose( fd );

	return true;
}
