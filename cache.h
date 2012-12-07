#ifndef __CACHE_H__
#define __CACHE_H__

//#include "common_stuff.h"

#include <map>
#include <QString>
#include "element.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <QMutex>
#include <QMutexLocker>

//#include "boost/shared_ptr.hpp"

//using Common::RESULT;

namespace Data
{
	typedef QList<TimeElem> TimeElemList;
	
	class Cache
	{
	protected:
		Cache(void);
		Cache(const QString& localDBPath);

	public:
		~Cache(void);

	public:
		typedef TimeElemList.Iterator iterator;

		static Cache* getCache(const QString& localDBPath = "");
		bool restoreCache();
		void insert(VFSElement& elem, bool dirty = false, bool db = true);
		bool erase(iterator& elem);
		bool erasePlugin(const QString& pluginName);
		bool flush();
		
		iterator find(const QString& key)
		{
			return m_elemList.find(key);
		}

		iterator begin()
		{
			return m_elemList.begin();
		}

		iterator end()
		{
			return m_elemList.end();
		}

	private:
		bool initDB();
	private:
		TimeElemList m_elemList;
		static QSqlDatabase m_db;
		static Cache* m_cacheInstance;
		static QString m_localDBPath;
	};
}

#endif // __CACHE_H__