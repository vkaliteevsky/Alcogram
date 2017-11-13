#include <QFile>
#include <QMap>
#include "configManager.h"

//Text

Text::Text(const QString &name, const QString &fontFamily, int size
           , qreal spacing, int weight, const QString &text)
    : _name(name)
    , _fontFamily(fontFamily)
    , _size(size)
    , _spacing(spacing)
    , _weight(weight)
    , _text(text)
{
}

QString Text::getName() const
{
    return _name;
}

QString Text::getFontFamily() const
{
    return _fontFamily;
}

int Text::getSize() const
{
    return _size;
}

qreal Text::getSpacing() const
{
    return _spacing;
}

int Text::getWeight() const
{
    return _weight;
}

QString Text::getText() const
{
    return _text;
}

//Config Manager

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , _currentLanguage("RU")
{

}

QString ConfigManager::getCurrentLanguage() const
{
    return _currentLanguage;
}

const QList<Text> ConfigManager::getTexts(const QString& page) const
{
    if (_currentLanguage == "RU") {
        if (_pageTextRU.contains(page)) {
            return _pageTextRU[page];
        }
    }

    if (_currentLanguage == "EN") {
        if (_pageTextEN.contains(page)) {
            return _pageTextEN[page];
        }
    }

    // TODO: logging, language dependent page text not found

    return QList<Text>();
}

const QList<Text> ConfigManager::getLanguageIndependentText(const QString& page) const
{
    if (_pageText.contains(page)) {
        return _pageText[page];
    }

    // TODO: logging, language independent page text not found

    return QList<Text>();
}

int ConfigManager::getTimeDuration(const QString& pageName, const QString& durationName) const
{
    if (_durations.contains(pageName)) {
        QMap<QString, int> pageDurations = _durations[pageName];

        if (pageDurations.contains(durationName)) {
            return pageDurations[durationName];
        }

        // TODO: logging, duration name not found

        return 0;
    }

    // TODO: logging, page not found

    return 0;
}

void ConfigManager::load(const QString& fileName)
{
    QFile* file = new QFile(fileName);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        // TODO: logging
        return;
    }

    QDomDocument domDocument;

    QString errorMessage;
    int errorLine;
    int errorColumn;

    bool error = !domDocument.setContent(file, &errorMessage, &errorLine, &errorColumn);

    if (error) {
        //  TODO: logging, errors described in QXmlParseException class documentation
        return;
    }


    QDomElement topElement = domDocument.documentElement();
    QDomNode domNode = topElement.firstChild();

    while (!domNode.isNull()) {
        QDomElement domElement = domNode.toElement();

        if (domElement.tagName() == "pages") {
            parsePages(domElement.firstChild());
        }

        domNode = domNode.nextSibling();
    }

    delete file;
}

void ConfigManager::parsePages(QDomNode page)
{
    // iterate over <page> tags
    while (!page.isNull()) {
        QDomElement pageElement = page.toElement();
        QString pageName = pageElement.attribute("name", "");

        _pageTextRU.insert(pageName, QList<Text>());
        _pageTextEN.insert(pageName, QList<Text>());
        _pageText.insert(pageName, QList<Text>());

        QDomNode pageSet = page.firstChild();

        // iterate over page settings tags
        while (!pageSet.isNull()) {
            QDomElement pageSetElement = pageSet.toElement();

            if (pageSetElement.tagName() == "texts") {
                parseTexts(pageSetElement.firstChild(), pageName);
            }

            if (pageSetElement.tagName() == "durations") {
                parseDurations(pageSetElement.firstChild(), pageName);
            }

            pageSet = pageSet.nextSibling();
        }

        page = page.nextSibling();
    }
}

void ConfigManager::parseTexts(QDomNode language, const QString& pageName)
{
    // iterate over <language> tags
    while (!language.isNull()) {
        QDomElement languageElement = language.toElement();
        QString languageName = languageElement.attribute("name", "");
        QDomNode text = languageElement.firstChild();

        // iterate over <text> tags
        while (!text.isNull()) {
            QDomElement textElement = text.toElement();

            Text t = Text(textElement.attribute("name", "")
                          , textElement.attribute("fontFamily", "Proxima Nova Rg")
                          , textElement.attribute("size", "0").toInt()
                          , textElement.attribute("spacing", "0").toDouble()
                          , textElement.attribute("weight", "75").toInt()
                          , textElement.text().remove(QRegExp("[\\n\\t\\r]")).simplified());

            if (languageName == "RU") {
                _pageTextRU[pageName].append(t);
            } else if (languageName == "EN") {
                _pageTextEN[pageName].append(t);
            } else {
                _pageText[pageName].append(t);
            }

            text = text.nextSibling();
        }

        language = language.nextSibling();
    }
}

void ConfigManager::parseDurations(QDomNode duration, const QString &pageName)
{
    // iterate over <duration> tags
    while (!duration.isNull()) {
        QDomElement durationElement = duration.toElement();
        QString durationName = durationElement.attribute("name", "");
        int time = durationElement.attribute("value", "0").toInt();

        if (!_durations.contains(pageName)) {
            _durations.insert(pageName, QMap<QString, int>());
        }

        _durations[pageName].insert(durationName, time);

        duration = duration.nextSibling();
    }
}