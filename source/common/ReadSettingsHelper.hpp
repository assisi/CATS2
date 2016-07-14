#ifndef CATS2_READ_SETTINGS_HELPER_HPP
#define CATS2_READ_SETTINGS_HELPER_HPP

#include <opencv2/opencv.hpp>

#include <QtCore/QDebug>
#include <QtCore/QString>

/*!
 * \brief A simple wrapper around the OpenCV's FileStorage class. Simplifies the search for a value
 * in the settings .XML file.
 */
class ReadSettingsHelper
{
public:
    //! Constructor.
    explicit ReadSettingsHelper(QString fileName)
    {
        _fs = cv::FileStorage(fileName.toStdString(), cv::FileStorage::READ);

        if (!_fs.isOpened())
            qDebug() << QString("Could not open the configuration file: %1").arg(fileName);
    }

    //! Desctructor.
    virtual ~ReadSettingsHelper()
    {
        _fs.release();
    }

    //! Read the variable value with the given path starting from the document's root node.
    //! If the path is not found then the variable gets the default value when provided.
    template <typename T>
    void readVariable(QString pathInFile, T& value, T defaultValue = T())
    {
        value = defaultValue;

        if (!_fs.isOpened()){
            qDebug() << QString("Could not read the configuration file");
            return;
        }

        QStringList pathInFileList = pathInFile.split("/");
        cv::FileNode node = _fs.root();
        foreach (QString nodeName, pathInFileList){
            node = node[nodeName.toStdString()];
            if (node.empty())
            {
                qDebug() << QString("Could not find requested parameters %1, taking default value instead").arg(pathInFile);
                return;
            }
        }
        node >> value;
    }
private:
    //! The file storage.
    cv::FileStorage _fs;
};

#endif // CATS2_READ_SETTINGS_HELPER_HPP
