/*
 * GameFolder.h
 *
 *  Created on: 12 dec. 2014
 *      Author: Jeromnimo
 */

#ifndef _GAMEFOLDER_H_
#define _GAMEFOLDER_H_

#include <map>
#include <set>

#include "GameFile.h"

#include "metaclasses/Container.h"

namespace core
{
  class GameConfig
  {
    public:
      QString locale;
      QString version;
      QString product;
  };

  class GameFolder : public Container<GameFile>
  {
    public:
      explicit GameFolder(const QString & path);
      virtual ~GameFolder() {}

      virtual void init() = 0;
      virtual void initFromListfile(const QString & file) = 0;
      virtual void addCustomFiles(const QString & path, bool bypassOriginalFiles) = 0;

      // return full path for a given file ie :
      // HumanMale.m2 => Character\Human\male\humanmale.m2
      // (not always accurate, as file names not always unique)
      QString getFullPathForFile(QString file);

      void getFilesForFolder(std::vector<GameFile *> &fileNames, QString folderPath, QString extension = "");
      void getFilteredFiles(std::set<GameFile *> &dest, QString & filter);
      GameFile * getFile(QString filename);
      virtual GameFile * getFile(int id) = 0;

      virtual bool openFile(std::string file, void ** result) = 0;
      virtual bool openFile(int id, void ** result) = 0;
      
      virtual QString version() = 0;
      virtual int majorVersion() = 0;
      virtual QString locale() = 0;
      virtual bool setConfig(GameConfig config) = 0;
      virtual std::vector<GameConfig> configsFound() = 0;

      virtual int lastError() = 0;

      virtual void onChildAdded(GameFile *) override;
      virtual void onChildRemoved(GameFile *) override;

      QString path() { return m_path; }

    private:
      std::map<QString, GameFile *> m_nameMap;
      QString m_path;
  };
}




#endif /* _GAMEFOLDER_H_ */
