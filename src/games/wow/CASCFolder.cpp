/*
 * CASCFolder.cpp
 *
 *  Created on: 22 oct. 2014
 *      Author: Jeromnimo
 */

#include "CASCFolder.h"

#ifndef __CASCLIB_SELF__
  #define __CASCLIB_SELF__
#endif
#include "CascLib.h"

#include <locale>
#include <map>
#include <utility>

#include <QFile>
#include <QRegularExpression>

#include "CASCFile.h"
#include "Logger.h"

CASCFolder::CASCFolder()
 : m_currentCascLocale(CASC_LOCALE_NONE), m_folder(""), m_openError(ERROR_SUCCESS), hStorage(nullptr)
{

}

void CASCFolder::init(const QString &path)
{
  m_folder = path;

  if(m_folder.endsWith("\\"))
    m_folder.remove(m_folder.size()-1,1);

  initBuildInfo();
}

bool CASCFolder::setConfig(core::GameConfig config)
{
  m_currentConfig = config;

  // init map based on CASCLib
  std::map<QString, int> locales;
  locales["frFR"] = CASC_LOCALE_FRFR;
  locales["deDE"] = CASC_LOCALE_DEDE;
  locales["esES"] = CASC_LOCALE_ESES;
  locales["esMX"] = CASC_LOCALE_ESMX;
  locales["ptBR"] = CASC_LOCALE_PTBR;
  locales["itIT"] = CASC_LOCALE_ITIT;
  locales["ptPT"] = CASC_LOCALE_PTPT;
  locales["enGB"] = CASC_LOCALE_ENGB;
  locales["ruRU"] = CASC_LOCALE_RURU;
  locales["enUS"] = CASC_LOCALE_ENUS;
  locales["enCN"] = CASC_LOCALE_ENCN;
  locales["enTW"] = CASC_LOCALE_ENTW;
  locales["koKR"] = CASC_LOCALE_KOKR;
  locales["zhCN"] = CASC_LOCALE_ZHCN;
  locales["zhTW"] = CASC_LOCALE_ZHTW;

  // set locale
  if (!m_currentConfig.locale.isEmpty())
  {
    auto it = locales.find(m_currentConfig.locale);

    if (it != locales.end())
    {
      HANDLE dummy;
      QString cascParams = m_folder + ":" + m_currentConfig.product;
      LOG_INFO << "Loading Game Folder:" << cascParams;
      // locale found => try to open it
      if (!CascOpenStorage(cascParams.toStdWString().c_str(), it->second, &hStorage))
      {
        m_openError = GetLastError();
        LOG_ERROR << "CASCFolder: Opening" << cascParams << "failed." << "Error" << m_openError;
        return false;
      }

      addExtraEncryptionKeys();

      if (CascOpenFile(hStorage, "Interface\\FrameXML\\Localization.lua", it->second, 0, &dummy))
      {
        CascCloseFile(dummy);
        m_currentCascLocale = it->second;
        LOG_INFO << "Locale succesfully set:" << m_currentConfig.locale;
      }
      else
      {
        LOG_ERROR << "Setting Locale" << m_currentConfig.locale << "for folder" << m_folder << "failed";
        return false;
      }
    }
  }

  return true;
}

void CASCFolder::initBuildInfo()
{
  QString buildinfofile = m_folder + "\\..\\.build.info";
  LOG_INFO << "buildinfofile : " << buildinfofile;

  QFile file(buildinfofile);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    LOG_ERROR << "Fail to open .build.info to grab game config info";
    return;
  }

  QTextStream in(&file);
  QString line;

  // read first line and grab VERSION index
  line = in.readLine();

  QStringList headers = line.split('|');
  int activeIndex = 0;
  int versionIndex = 0;
  int tagIndex = 0;
  int productIndex = 0;
  for (int index = 0; index < headers.size(); index++)
  {
    if (headers[index].contains("Active", Qt::CaseInsensitive))
      activeIndex = index;
    else if (headers[index].contains("Version", Qt::CaseInsensitive))
      versionIndex = index;
    else if (headers[index].contains("Tags", Qt::CaseInsensitive))
      tagIndex = index;
    else if (headers[index].contains("Product", Qt::CaseInsensitive))
      productIndex = index;
  }

  // now loop across file lines with actual values
  while (in.readLineInto(&line))
  {
    QString version, product;
    QStringList values = line.split('|');

    // if inactive config, skip it
    if (values[activeIndex] == "0")
      continue;

    // grab version for this line
    QRegularExpression re("^(\\d).(\\d).(\\d).(\\d+)");
    QRegularExpressionMatch result = re.match(values[versionIndex]);
    if (result.hasMatch())
      version = result.captured(1) + "." + result.captured(2) + "." + result.captured(3) + "." + result.captured(4);

    // grab product name for this line
    product = values[productIndex];

    // grab locale(s) for this line
    values = values[tagIndex].split(':');
    for (int i = 0; i < values.size(); i++)
    {
      if (values[i].contains("text?"))
      {
        QStringList tags = values[i].split(" ");
        core::GameConfig config;
        config.locale = tags[tags.size() - 2];
        config.version = version;
        config.product = product;
        m_configs.push_back(config);
      }
    }

  }

  for (auto it : m_configs)
    LOG_INFO << "config" << it.locale << it.version;
}


bool CASCFolder::fileExists(int id)
{
  //LOG_INFO << __FUNCTION__ << " " << file.c_str();
  if(!hStorage)
    return false;

  HANDLE dummy;

  if(CascOpenFile(hStorage, CASC_FILE_DATA_ID(id), m_currentCascLocale, CASC_OPEN_BY_FILEID, &dummy))
  {
   // LOG_INFO << "OK";
    CascCloseFile(dummy);
    return true;
  }
  // LOG_ERROR << "File" << id << "doesn't exist." << "Error" << GetLastError();
  return false;
}

bool CASCFolder::openFile(int id, HANDLE * result)
{
  return CascOpenFile(hStorage, CASC_FILE_DATA_ID(id), m_currentCascLocale, CASC_OPEN_BY_FILEID, result);
}

bool CASCFolder::closeFile(HANDLE file)
{
  return CascCloseFile(file);
}

void CASCFolder::addExtraEncryptionKeys()
{
  QFile tactKeys("extraEncryptionKeys.csv");

  if (tactKeys.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&tactKeys);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      if (line.startsWith("##") || line.startsWith("\"##"))  // ignore lines beginning with ##, useful for adding comments.
        continue;
        
      QStringList lineData = line.split(';');
      if (lineData.size() != 2)
        continue;
      QString keyName = lineData.at(0);
      QString keyValue = lineData.at(1);
      if (keyName.isEmpty() || keyValue.isEmpty())
        continue;

      bool ok, ok2;
      ok2 = CascAddStringEncryptionKey(hStorage, keyName.toULongLong(&ok, 16), keyValue.toStdString().c_str());
      if (ok2)
        LOG_INFO << "Adding TACT key from file, Name:" << keyName << ", Value:" << keyValue ;
      else
        LOG_ERROR << "Failed to add TACT key from file, Name:" << keyName << ", Value:" << keyValue ;  
    }
  }
}

/*
int CASCFolder::fileDataId(std::string & filename)
{
  return CascGetFileId(hStorage, filename.c_str());
}
*/
