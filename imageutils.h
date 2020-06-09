#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H


//to avoid compile error, as we dont directly include those classes (for better performances)

class QPixmap;
class QString;
class QStringList;

/**
  *@class ImageUtils
  *@brief This class contains utilities functions for images.
  */
class ImageUtils
{
public:
    ImageUtils();


    static QString      supportedImageFormatsFilter();
    static QStringList  supportedImageFormats();
    static QString      imageInfo(const QPixmap & image, const QString &fileName);
};

#endif // IMAGEUTILS_H
