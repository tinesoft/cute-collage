#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include <QLibraryInfo>
#include "application.h"

QTranslator* Application::m_currentAppTranslator = 0;
QTranslator* Application::m_currentQtTranslator = 0;

Translators Application::m_appTranslators;
Translators Application::m_qtTranslators;

Application::Application(int& argc, char* argv[])
	: QApplication(argc, argv)
{
}

Application::~Application()
{
}

void Application::loadTranslations(const QString& dir)
{
	loadTranslations(QDir(dir));
}

void Application::loadTranslations(const QDir& dir)
{
    // <appname>_<languagecode>.qm
    QString filter = "*_*.qm";
    QDir::Filters filters = QDir::Files | QDir::Readable;
    QDir::SortFlags sort = QDir::Name;
    QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);
    foreach (QFileInfo file, entries)
    {
        // pick country and language out of the file name
        QStringList parts = file.baseName().split("_");
        QString language  = parts.at(parts.count() - 1);

        // construct and load app translator
        QTranslator* appTranslator = new QTranslator(instance());
        if (appTranslator->load(file.absoluteFilePath()))
        {
            m_appTranslators.insert(language, appTranslator);
        }

        // construct and load qt translator
        QTranslator* qtTranslator = new QTranslator(instance());
        if(qtTranslator->load("qt_" + language, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        {
            m_qtTranslators.insert(language, qtTranslator);
        }


    }
}

const QStringList Application::availableLanguages()
{
	// the content won't get copied thanks to implicit sharing and constness
        return QStringList(m_appTranslators.keys());
}

void Application::setLanguage(const QString& locale)
{
        // remove previous application translator
        if (m_currentAppTranslator)
	{
                removeTranslator(m_currentAppTranslator);
	}

        // remove previous Qt translator
        if (m_currentQtTranslator)
        {
                removeTranslator(m_currentQtTranslator);
        }

        // install new  application translator
        m_currentAppTranslator = m_appTranslators.value(locale, 0);
        if (m_currentAppTranslator)
	{
                installTranslator(m_currentAppTranslator);
	}

        // install new  Qt translator
        m_currentQtTranslator = m_qtTranslators.value(locale, 0);
        if (m_currentQtTranslator)
        {
                installTranslator(m_currentQtTranslator);
        }
}
