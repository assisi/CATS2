#include "CommandLineParameters.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
CommandLineParameters::CommandLineParameters() :
    m_useSharedRobotInterface(true),
    m_useInterSpacesModule(true),
    m_useSettingsInterface(false)
{
}

/*!
 * Destructor.
 */
CommandLineParameters::~CommandLineParameters()
{
    qDebug() << "Destroying the object";
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
bool CommandLineParameters::init(int argc, char** argv, bool needConfigFile,
                                 bool needMainCamera, bool needBelowCamera)
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
    bool foundMainCameraParameters =
            (CommandLineParser::parseTypedArgument(argc, argv,
                                                   "-mc", streamType, streamParameters) ||
             CommandLineParser::parseTypedArgument(argc, argv,
                                                   "--maincam", streamType, streamParameters));
    if (foundMainCameraParameters) {
        // the main camera
        addCameraDescriptor(SetupType::MAIN_CAMERA,
                            StreamDescriptor(streamType, streamParameters));
    } else {
        qDebug() << "Couldn't find the valid main camera parameters";
    }
    settingsAccepted = (foundMainCameraParameters || (!needMainCamera));

    // read the below camera parameters
    bool foundBelowCameraParameters =
            (CommandLineParser::parseTypedArgument(argc, argv, "-bc", streamType, streamParameters) ||
             CommandLineParser::parseTypedArgument(argc, argv, "--belowcam", streamType, streamParameters));
    if (foundBelowCameraParameters) {
        // the below camera
        addCameraDescriptor(SetupType::CAMERA_BELOW,
                            StreamDescriptor(streamType, streamParameters));
    } else {
        qDebug() << "Couldn't find the valid camera below parameters";
    }
    settingsAccepted = settingsAccepted && (foundBelowCameraParameters || (!needBelowCamera));

    // get the configuration file path
    QString filePath;
    bool foundConfigFilePath =
            (CommandLineParser::parseArgument(argc, argv, "-c", filePath) ||
             CommandLineParser::parseArgument(argc, argv, "--config", filePath));
    if (foundConfigFilePath) {
        m_configurationFilePath = filePath;
    } else {
        qDebug() << "Couldn't find the configuration file";
    }
    settingsAccepted = settingsAccepted && (foundConfigFilePath || (!needConfigFile));

    // get the flag for the robot interface
    QString flag;
    bool foundRobotInterfaceFlag =
            (CommandLineParser::parseArgument(argc, argv, "-sri", flag) ||
             CommandLineParser::parseArgument(argc, argv, "--shared-robot-interface", flag));
    if (foundRobotInterfaceFlag) {
        m_useSharedRobotInterface = (flag.toInt() == 1);
    } else {
        qDebug() << "Couldn't find the robot interface flag, set  to default "
                    "value 'true' (connecting to the shared interface)";
        m_useSharedRobotInterface = true;
    }

    // get the flag for the interspaces communication
    bool foundInterSpacesFlag =
            (CommandLineParser::parseArgument(argc, argv, "-is", flag) ||
             CommandLineParser::parseArgument(argc, argv, "--inter-spaces", flag));
    if (foundInterSpacesFlag) {
        m_useInterSpacesModule = (flag.toInt() == 1);
    } else {
        qDebug() << "Couldn't find the inter-spaces flag, set  to default "
                    "value 'true' (inter-spaces activated)";
        m_useInterSpacesModule = true;
    }

    // get the flag for the settings interface
    bool foundSettingsInterfaceFlag =
            (CommandLineParser::parseArgument(argc, argv, "-si", flag) ||
             CommandLineParser::parseArgument(argc, argv, "--settings-interface", flag));
    if (foundSettingsInterfaceFlag) {
        m_useSettingsInterface = (flag.toInt() == 1);
    } else {
        qDebug() << "Couldn't find the settings interface flag, set  to default "
                    "value 'false' (settings interface is disabled)";
        m_useSettingsInterface = false;
    }

    // get the flag for the robot statistics publishing
    bool foundRobotStatisticsPublishingFlag =
            (CommandLineParser::parseArgument(argc, argv, "-sp", flag) ||
             CommandLineParser::parseArgument(argc, argv, "--statistics-publisher", flag));
    if (foundRobotStatisticsPublishingFlag) {
        m_publishRobotsStatistics = (flag.toInt() == 1);
    } else {
        qDebug() << "Couldn't find the statistics publishing flag, set  to "
                    "default value 'false' (statistics publishing is disabled)";
        m_publishRobotsStatistics = false;
    }

    return settingsAccepted;
}

/*!
 * Prints out the command line arguments expected by the video grabber.
 */
void CommandLineParameters::printSupportedArguments()
{
    qDebug() << "Video grabber usage";
    qDebug() << "\t -h --help\t\tShow this help message";
    qDebug() << "\t -mc --maincam\tDefines the video stream used to track agents. "
                "Format : -mc <StreamType> <parameters>";
    qDebug() << "\t -bc --belowcam\tDefines the video stream used to track the "
                "robot under the aquarium. Format : -mc <StreamType> <parameters>";
    qDebug() << "\t -c --config\tThe configuration file. Format : -c <PathToFile>";
    qDebug() << "\t -sri --shared-robot-interface\tThe flag to use a shared"
                "interface to connect to robots. Format : -sri 1/0, by default"
                "is on";
    qDebug() << "\t -is --inter-spaces\tThe flag to activate the interspaces "
                "module. Format : -is 1/0, by default is on";
    qDebug() << "\t -si --settings-interface\tThe flag to activate the remote"
                "access to the settings interface. Format : -si 1/0, by default"
                "is off";
    qDebug() << "\t -sp --statistics-publisher\tThe flag to activate the "
                "puplishing of the robots statistics. Format : -sp 1/0, by "
                "default is off";
}

/*!
 * Looks for the given stream's argument in the command line.
 */
bool CommandLineParameters::parseStreamArguments(int argc, char** argv,
                                                 QString argument,
                                                 QString& streamType,
                                                 QString& parameters)
{
    QString argumentType;
    QString values;
    bool status = CommandLineParser::parseTypedArgument(argc, argv, argument, argumentType, values);
    if (status) {
        if (StreamDescriptor::isValidStreamType(argumentType)){
            streamType = argumentType;
            parameters = values;
            return true;
        } else {
            qDebug() << "Invalid argument type" << argumentType;
        }
    }
    return false;
}
