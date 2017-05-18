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
        m_fs = cv::FileStorage(fileName.toStdString(), cv::FileStorage::READ);

        if (!m_fs.isOpened())
            qDebug() << QString("Could not open the configuration file: %1").arg(fileName);
    }

    //! Desctructor.
    virtual ~ReadSettingsHelper()
    {
        if (m_fs.isOpened())
            close();
    }

    //! Close the file.
    void close()
    {
        m_fs.release();
    }

    //! Checks if the node exists. Used to check if certain sections exist in
    //! the settings.
    bool validPath(QString pathInFile)
    {
        if (!m_fs.isOpened()){
            qDebug() << QString("Could not read the configuration file");
            return false;
        }

        QStringList pathInFileList = pathInFile.split("/");
        cv::FileNode node = m_fs.root();
        foreach (QString nodeName, pathInFileList){
            node = node[nodeName.toStdString()];
            if (node.empty())
            {
                qDebug() << QString("Could not find requested section %1")
                            .arg(pathInFile);
                return false;
            }
        }
        // the node is a section (mapping)
        return node.type() == cv::FileNode::MAP;
    }

    //! Read the variable value with the given path starting from the document's root node.
    //! If the path is not found then the variable gets the default value when provided.
    // TODO : refactor this to return true/false depending if the value is read
    // this would simply a lot the handling of settings
    template <typename T>
    void readVariable(QString pathInFile, T& value, T defaultValue = T())
    {
        value = defaultValue;

        if (!m_fs.isOpened()){
            qDebug() << QString("Could not read the configuration file");
            return;
        }

        QStringList pathInFileList = pathInFile.split("/");
        cv::FileNode node = m_fs.root();
        foreach (QString nodeName, pathInFileList){
            node = node[nodeName.toStdString()];
            if (node.empty()) {
                qDebug() << QString("Could not find requested parameters %1, taking default value instead").arg(pathInFile);
                return;
            }
        }
        node >> value;
    }

    //! Read values of all variables having the same type and name, used for the
    //! lists. The path starts from the document's root node.
    //! If the path is not found then an empty list is returned.
    // TODO : refactor this to return true/false depending if the value is read
    // this would simply a lot the handling of settings
    template <typename T>
    void readVariables(QString pathInFile, std::vector<T>& values)
    {
        values = std::vector<T>();

        if (!m_fs.isOpened()){
            qDebug() << QString("Could not read the configuration file");
            return;
        }

        QStringList pathInFileList = pathInFile.split("/");
        if (pathInFileList.isEmpty())
            return;

        // reach the level just above the variable
        cv::FileNode node = m_fs.root();
        while (pathInFileList.size() > 1) {
            QString nodeName = pathInFileList.takeFirst();
            node = node[nodeName.toStdString()];
            if (node.empty()) {
                qDebug() << QString("Could not find requested parameters %1, taking default value instead").arg(pathInFile);
                return;
            }
        }
        // loop for all the variables with the same name
        std::string nodeName = pathInFileList.first().toStdString();
        for(cv::FileNodeIterator it = node.begin() ; it!= node.end(); ++it)
        {
            if ((*it).name() == nodeName) {
                T value;
                (*it) >> value;
                values.push_back(value);
            }
        }
    }

private:
    //! The file storage.
    cv::FileStorage m_fs;
};

#endif // CATS2_READ_SETTINGS_HELPER_HPP
