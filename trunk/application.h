#ifndef APPLICATION_H

#include <QApplication>
#include <QHash>
#include <QStringList>

class QDir;
class QTranslator;

typedef QHash<QString, QTranslator*> Translators;

/**
  * @class Application
  * @brief
  * This class inherits of QApplication, and is
  * provided for allowing dynamic translation
  * of the whole application.
  */
class Application : public QApplication
{
	Q_OBJECT

public:
	explicit Application(int& argc, char* argv[]);
	~Application();

	static void loadTranslations(const QString& dir);
	static void loadTranslations(const QDir& dir);
	static const QStringList availableLanguages();

public slots:
	static void setLanguage(const QString& locale);

private:
        static QTranslator * m_currentAppTranslator;
        static QTranslator * m_currentQtTranslator;

        static Translators m_appTranslators;
        static Translators m_qtTranslators;
};

#endif // APPLICATION_H
