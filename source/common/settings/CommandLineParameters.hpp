#ifndef CATS2_COMMAND_LINE_PARAMETERS_HPP
#define CATS2_COMMAND_LINE_PARAMETERS_HPP

#include "SetupType.hpp"
#include "CommandLineParser.hpp"
#include "StreamDescriptor.hpp"

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>

/*!
 * Class-signleton that is used to store parameters of the grabber; at the moment
 * those are just input streams types and attributes.
 * Their values are received from the command line arguments.
 * NOTE : in C++11 there is no need for manual locking. Concurrent execution
 * shall wait if a static local variable is already being initialized.
 * Based on http://stackoverflow.com/questions/11711920/how-to-implement-
 * multithread-safe-singleton-in-c11-without-using-mutex
 */
class CommandLineParameters
{
public:
    //! The singleton getter.
    static CommandLineParameters& get();

    //! Initializes the parameters from the command line arguments. Returns true
    //! if at least the main camera parameter is found and there were no "-h"
    //! option. It receives also flags difining which command line parameters
    //! are considired essential so that the initialisation passes.
    bool init(int argc, char** argv, bool needConfigFile = true,
              bool needMainCamera = true,
              bool needBelowCamera = false);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    CommandLineParameters(CommandLineParameters const&) = delete;
    //! Move constructor.
    CommandLineParameters(CommandLineParameters&&) = delete;
    //! Copy assignment.
    CommandLineParameters& operator=(CommandLineParameters const&) = delete;
    //! Move assignment.
    CommandLineParameters& operator=(CommandLineParameters &&) = delete;

public:
    //! Add the camera descriptor for the given setup.
    void addCameraDescriptor(SetupType::Enum setupType,
                             StreamDescriptor descriptor)
    {
        m_cameraDescriptors[setupType] = descriptor;
    }

    //! Returns the parameters of the given setup.
    StreamDescriptor cameraDescriptor(SetupType::Enum setupType) const
    {
        return m_cameraDescriptors[setupType];
    }
    //! Sets the path to the configuration file.
    void setConfigurationFilePath(QString path) { m_configurationFilePath = path; }
    //! Return the path to the configuration file.
    QString configurationFilePath() const { return m_configurationFilePath; }
    //! Returns the list of the setup types correspondign to provided input
    //! streams.
    QList<SetupType::Enum> availableSetups() const
    {
        return m_cameraDescriptors.keys();
    }
    //! Returns the flag defining wether one interface is shared by all robots.
    bool useSharedRobotInterface() const { return m_useSharedRobotInterface; }
    //! Returns the flag defining wether the inter-spaces module is to be
    //! activated.
    bool useInterSpacesModule() const { return m_useInterSpacesModule; }

private:
    //! Constructor. Defining it here prevents construction.
    CommandLineParameters();
    //! Destructor. Defining it here prevents unwanted destruction.
    ~CommandLineParameters();

private:
    //! Prints out the command line arguments expected by the video grabber.
    void printSupportedArguments();
    //! Looks for the given stream's argument in the command line.
    bool parseStreamArguments(int argc, char** argv,
                              QString argument,
                              QString& streamType,
                              QString& parameters);

private:
    //! Input streams parameters for different setups.
    QMap<SetupType::Enum, StreamDescriptor> m_cameraDescriptors;

    //! The path to the configuration file.
    QString m_configurationFilePath;

    //! Defines if one interface is shared by all robots or is created exclusively
    //! for every robot.
    bool m_useSharedRobotInterface;
    //! Defines if the inter-spaces module is to be activated.
    bool m_useInterSpacesModule;
};

#endif // CATS2_COMMAND_LINE_PARAMETERS_HPP
