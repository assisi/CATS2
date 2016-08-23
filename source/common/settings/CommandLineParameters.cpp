#include "CommandLineParameters.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
CommandLineParameters::CommandLineParameters()
{
}

/*!
 * Destructor.
 */
CommandLineParameters::~CommandLineParameters()
{

}

/*!
 * The singleton getter.
 */
CommandLineParameters& CommandLineParameters::get()
{
    static CommandLineParameters instance;
    return instance;
}

/*!
 * Initializes the parameters from the command line arguments.
 * This part is inspired by this stackoverflow post
 * http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
 */
bool CommandLineParameters::init(int argc, char** argv, bool needConfigFile, bool needMainCamera, bool needBelowCamera)
{
    bool settingsAccepted = false;

    // first check for the "-h"/"--help" request
    if ((std::find(argv, argv + argc, "-h") != (argv + argc))
        || (std::find(argv, argv + argc, "--help") != (argv + argc))) {
        printSupportedArguments();
        // if the user asks for a help then he/she doesn't know what is happening,
        // hence need to ignore the rest of arguments
        return settingsAccepted;
    }

    // read the main camera parameters
    QString streamType;
    QString streamParameters;
    bool foundMainCameraParameters = (parseStreamArguments(argc, argv, "-mc", streamType, streamParameters)
                                      || parseStreamArguments(argc, argv, "--maincam", streamType, streamParameters));
    if (foundMainCameraParameters) {
        // the main camera
        m_cameraDescriptors[SetupType::MAIN_CAMERA] = StreamDescriptor(streamType, streamParameters);
    }
    settingsAccepted = (foundMainCameraParameters || (!needMainCamera));

    // read the below camera parameters
    bool foundBelowCameraParameters = (parseStreamArguments(argc, argv, "-bc", streamType, streamParameters)
                                      || parseStreamArguments(argc, argv, "--belowcam", streamType, streamParameters));
    if (foundBelowCameraParameters) {
        // the below camera
        m_cameraDescriptors[SetupType::CAMERA_BELOW] = StreamDescriptor(streamType, streamParameters);
    }
    settingsAccepted = settingsAccepted && (foundBelowCameraParameters || (!needBelowCamera));

    // get the configuration file path
    QString filePath;
    bool foundConfigFilePath = (parseConfigFilePath(argc, argv, "-c", filePath)
                                      || parseConfigFilePath(argc, argv, "--config", filePath));
    if (foundConfigFilePath) {
        m_configurationFilePath = filePath;
    }
    settingsAccepted = settingsAccepted && (foundConfigFilePath || (!needConfigFile));

    return settingsAccepted;
}

/*!
 * Prints out the command line arguments expected by the video grabber.
 */
void CommandLineParameters::printSupportedArguments()
{
    qDebug() << "Video grabber usage";
    qDebug() << "\t -h --help\t\tShow this help message";
    qDebug() << "\t -mc --maincam\tDefines the video stream used to track agents. Format : -mc <StreamType> <parameters>";
    qDebug() << "\t -bc --belowcam\tDefines the video stream used to track the robot under the aquarium. Format : -mc <StreamType> <parameters>";
    qDebug() << "\t -c --config\tThe configuration file. Format : -c <PathToFile>";
}

/*!
 * Looks for the given argument in the command line, if found it return second and third argument as
 * stream type and parameters.
 */
bool CommandLineParameters::parseStreamArguments(int argc, char** argv, QString argument, QString& streamType, QString& parameters)
{
    char** itr = std::find(argv, argv + argc, argument);
    if ((itr != (argv + argc)) && ((itr + 1) != (argv + argc)) && ((itr + 2) != (argv + argc))) {
        QString value = QString(*(itr + 1)).toLower();
        if (StreamDescriptor::isValidStreamType(value)){
            streamType = value;
            parameters = *(itr + 2);
            return true;
        }
    }
    return false;
}

/*!
 * Looks for the given argument in the command line, if found it return the argument as configuration file name.
 */
bool CommandLineParameters::parseConfigFilePath(int argc, char** argv, QString argument, QString& filePath)
{
    char** end = argv + argc;
    char** itr = std::find(argv, end, argument);
    if ((itr != end) && (++itr != end)) {
        filePath = QString(*itr);
        return true;
    }
    return false;
}
