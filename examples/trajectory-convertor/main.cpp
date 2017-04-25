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
 * Extracts a trajectory for one fish from the idTracker resulted file.
 */
int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("EPFL-LSRO-Mobots");
    QApplication::setOrganizationDomain("mobots.epfl.ch");
    QApplication::setApplicationName("CATS2-trajectory-convertor");

    QGst::init(nullptr, nullptr);
    QApplication app(argc, argv);

   // parse input arguments to initialize the settings
    if (CommandLineParameters::get().init(argc, argv, true, false, false)) {
        // get the trajectory image width
        QString sourceWidthString;
        bool foundSourceWidth =
                (CommandLineParser::parseArgument(argc, argv, "-sw", sourceWidthString) ||
                 CommandLineParser::parseArgument(argc, argv, "--source-weight", sourceWidthString));
        // get the trajectory image height
        QString sourceHeightString;
        bool foundSourceHeight =
                (CommandLineParser::parseArgument(argc, argv, "-sh", sourceHeightString) ||
                 CommandLineParser::parseArgument(argc, argv, "--source-hegiht", sourceHeightString));
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
        // get the fish number
        QString fishNumberString;
        bool foundFishNumber =
                (CommandLineParser::parseArgument(argc, argv, "-fn", fishNumberString) ||
                 CommandLineParser::parseArgument(argc, argv, "--fish-number", fishNumberString));
        // get the chosen fish index
        QString fishIndexString;
        bool foundFishIndex =
                (CommandLineParser::parseArgument(argc, argv, "-fi", fishIndexString) ||
                 CommandLineParser::parseArgument(argc, argv, "--fish-index", fishIndexString));

        if (foundSourceWidth && foundSourceHeight && foundSetupType &&
            foundTrajectoryFile && foundFishNumber && foundFishIndex)
        {
            qDebug() << fishNumberString << setupTypeString
                     << sourceHeightString << sourceWidthString;

            SetupType::Enum setupType = SetupType::fromSettingsString(setupTypeString);
            if (setupType != SetupType::UNDEFINED) {
                CoordinatesConversionPtr coordinatesConversion;
                // check that the calibration settings are valid
                if (CalibrationSettings::get().init(CommandLineParameters::get().configurationFilePath(), setupType)) {
                    // create the camera calibration
                    coordinatesConversion = CoordinatesConversionPtr(new CoordinatesConversion(CalibrationSettings::get().calibrationFilePath(setupType),
                                                                                               CalibrationSettings::get().frameSize(setupType)));
                }

                double widthScale =  sourceWidthString.toDouble() / CalibrationSettings::get().frameSize(setupType).width();
                double heightScale =  sourceHeightString.toDouble() / CalibrationSettings::get().frameSize(setupType).height();
                int fishIndex = fishIndexString.toInt();

                // read the file here
                QFileInfo inputFileInfo(trajectoryFilePath);
                QFile inputFile(trajectoryFilePath);

                if (!inputFile.open(QIODevice::ReadOnly)) {
                    qDebug() << "Can't open input file: " << trajectoryFilePath;
                    return 0;
                }

                QTextStream inputStream(&inputFile);

                // previous position to use when NaN positions arrive
                QList<PositionMeters> trajectoryWorld;
                PositionPixels currentPositionImage;
                PositionPixels previousPositionImage;
                previousPositionImage.setValid(false);
                // skip the first line with headers
                if (!inputStream.atEnd())
                    inputStream.readLine();
                // read the rest of lines
                while (!inputStream.atEnd()) {
                    QString line = inputStream.readLine();
                    QStringList values = line.split("\t");
                    if (values.size() >= fishIndex*3) {
                        // read the coordinates
                        QString xImageString = values[3* (fishIndex - 1)];
                        QString yImageString = values[3* (fishIndex - 1) + 1];
                        // check for nans
                        if (xImageString.toLower() == "nan" || yImageString.toLower() == "nan") {
                            if (previousPositionImage.isValid()) {
                                currentPositionImage = previousPositionImage;
                                currentPositionImage.setValid(true);
                            } else {
                                currentPositionImage.setValid(false);
                            }
                        } else {
                            currentPositionImage.setX(xImageString.toDouble() / widthScale);
                            currentPositionImage.setY(yImageString.toDouble() / heightScale);
                            currentPositionImage.setValid(true);
                        }
                        // convert the position to world coordinates
                        trajectoryWorld.append(coordinatesConversion->imageToWorldPosition(currentPositionImage));
                        // update the previous position
                        if (currentPositionImage.isValid())
                            previousPositionImage = currentPositionImage;
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

