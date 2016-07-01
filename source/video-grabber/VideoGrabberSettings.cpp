#include "VideoGrabberSettings.hpp"

/*!
 * Constructor.
 */
VideoGrabberSettings::VideoGrabberSettings()
{
}

/*!
 * Destructor.
 */
VideoGrabberSettings::~VideoGrabberSettings()
{

}

/*!
 * The singleton getter.
 */
VideoGrabberSettings& VideoGrabberSettings::get()
{
    static VideoGrabberSettings instance;
    return instance;
}

/*!
 * Initializes the parameters from the command line arguments.
 * This part is inspired by this stackoverflow post
 * http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
 */
bool VideoGrabberSettings::init(int argc, char** argv)
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

    // try to find the supported arguments among present ones
    QString streamType;
    QString streamParameters;
    bool foundMainCameraParameters = (parseStreamArguments(argc, argv, "-mc", streamType, streamParameters)
                                      || parseStreamArguments(argc, argv, "--maincam", streamType, streamParameters));
    if (foundMainCameraParameters) {
        _mainCameraDescriptor = StreamDescriptor(streamType, streamParameters);
        settingsAccepted = foundMainCameraParameters;
    }
    return settingsAccepted;
}

/*!
 * Prints out the command line arguments expected by the video grabber.
 */
void VideoGrabberSettings::printSupportedArguments()
{
    qDebug() << "Video grabber usage";
    qDebug() << "\t -h --help\t\tShow this help message";
    qDebug() << "\t -mc --maincam\tDefines the video stream used to track agents. Format : -mc <StreamType> <parameters>";
}

/*!
 * Looks for the given argument in the command line, if found it return second and third argument as
 * stream type and parameters.
 */
bool VideoGrabberSettings::parseStreamArguments(int argc, char** argv, QString argument, QString& streamType, QString& parameters)
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
