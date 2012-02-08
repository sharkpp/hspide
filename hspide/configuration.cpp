#include "configuration.h"

Configuration::Configuration()
{
}

Configuration::Configuration(const Configuration& info)
{
	*this = info;
}

Configuration& Configuration::operator = (const Configuration& info)
{
	m_hspPath       = info.m_hspPath;
	m_hspCommonPath = info.m_hspCommonPath;
	m_compilerPath  = info.m_compilerPath;

	m_editorTabWidth = info.m_editorTabWidth;
	m_editorLineNumberVisibled = info.m_editorLineNumberVisibled;

	m_editorLineNumberFontName = info.m_editorLineNumberFontName;
	m_editorLineNumberFontSize = info.m_editorLineNumberFontSize;
	m_editorFontName = info.m_editorFontName;
	m_editorFontSize = info.m_editorFontSize;

	for(int i = 0; i < MetricsNum; i++) {
		m_metrics[i] = info.m_metrics[i];
	}

	m_tools = info.m_tools;

	emit updateConfiguration(*this);

	return *this;
}

