#include <settings/CommandLineParameters.hpp>
#include <settings/CommandLineParser.hpp>
#include <settings/CalibrationSettings.hpp>
#include <settings/GrabberSettings.hpp>
#include <settings/ViewerSettings.hpp>
#include <CoordinatesConversion.hpp>
#include <CommonPointerTypes.hpp>
#include <GrabberPointerTypes.hpp>
#include <GrabberData.hpp>
#include <GrabberHandler.hpp>
#include <gui/ViewerWindow.hpp>
#include <AgentState.hpp>

#include <QGst/Init>

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

/*!
 * Reads a text file with positions in screen coordinates, one position by line,
 * x and y separated by tab.
 * The setup type is either 'mainCamera' or 'cameraBelow'.
 */
int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-coordinates-convertor");

    QGst::init(nullptr, nullptr);
    QApplication app(argc, argv);

   // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        // defines if the first line in the file is the header
        QString containsHeaderString;
        bool foundHeaderFlag =
                (CommandLineParser::parseArgument(argc, argv, "-wh", containsHeaderString) ||
                 CommandLineParser::parseArgument(argc, argv, "--with-header", containsHeaderString));
        bool skipHeader = false;
        if (foundHeaderFlag)
            skipHeader = static_cast<bool>(containsHeaderString.toInt());
        // get the setup type
        QString setupTypeString;
        bool foundSetupType =
                (CommandLineParser::parseArgument(argc, argv, "-st", setupTypeString) ||
                 CommandLineParser::parseArgument(argc, argv, "--setup-type", setupTypeString));
        // get the trajectory file
        QString trajectoryFilePath;
        bool foundTrajectoryFile =
                (CommandLineParser::parseArgument(argc, argv, "-t", trajectoryFilePath) ||
                 CommandLineParser::parseArgument(argc, argv, "--trajectory", trajectoryFilePath));

        if (foundSetupType && foundTrajectoryFile)
        {
            SetupType::Enum setupType = SetupType::fromSettingsString(setupTypeString);
            if (setupType != SetupType::UNDEFINED) {
                CoordinatesConversionPtr coordinatesConversion;
                // check that the calibration settings are valid
                if (CalibrationSettings::get().init(CommandLineParameters::get().configurationFilePath(), setupType)) {
                    // create the camera calibration
                    coordinatesConversion = CoordinatesConversionPtr(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType),
                                                                                               CalibrationSettings::get().frameSize(setupType)));
                }

                // read the file here
                QFileInfo inputFileInfo(trajectoryFilePath);
                QFile inputFile(trajectoryFilePath);

                if (!inputFile.open(QIODevice::ReadOnly)) {
                    qDebug() << "Can't open input file: " << trajectoryFilePath;
                    return 0;
                }

                QTextStream inputStream(&inputFile);

                PositionPixels positionImage;
                // skip the first line with headers
                if (!inputStream.atEnd() && skipHeader)
                    inputStream.readLine();
                // read the rest of lines
                QList<PositionMeters> trajectoryWorld;
                while (!inputStream.atEnd()) {
                    QString line = inputStream.readLine();
                    QStringList values = line.split("\t");
                    if (values.size() >= 2) {
                        // read the coordinates
                        QString xImageString = values[0];
                        QString yImageString = values[1];
                        positionImage.setX(xImageString.toDouble());
                        positionImage.setY(yImageString.toDouble());
                        positionImage.setValid(true);
                        // convert the position to world coordinates
                        trajectoryWorld.append(coordinatesConversion->imageToWorldPosition(positionImage));
                    }
                }
                inputFile.close();

                // we store it to the output xml file
                std::vector<cv::Point2d> worldPoints;
                for (auto& worldPoint : trajectoryWorld) {
                    worldPoints.push_back(cv::Point2d(worldPoint.x(), worldPoint.y()));
                }
                QString outputFileName = inputFileInfo.absolutePath() + QDir::separator() +
                        inputFileInfo.baseName() + "World.xml";
                cv::FileStorage fs(outputFileName.toStdString(), cv::FileStorage::WRITE);
                fs << "polygon" << worldPoints;
                qDebug() << QString("stored %1 points").arg(worldPoints.size());

                // and also to the txt file for debug purposed
                outputFileName = inputFileInfo.absolutePath() + QDir::separator() +
                        inputFileInfo.baseName() + "World.txt";
                QFile outputFile(outputFileName);
                if (outputFile.open(QFile::WriteOnly | QFile::Truncate)) {
                    QTextStream outputStream(&outputFile);
                    for (auto& worldPoint : trajectoryWorld) {
                        outputStream << worldPoint.x() << "\t" << worldPoint.y() << "\n";
                    }
                }
                outputFile.close();
            }
        }
    } else {
        qDebug() << "Couldn't find necessary input arguments, finished";
    }
}

