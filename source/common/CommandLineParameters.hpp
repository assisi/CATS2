#ifndef CATS2_COMMAND_LINE_PARAMETERS_HPP
#define CATS2_COMMAND_LINE_PARAMETERS_HPP

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>

/*!
 * \brief The type of the incoming video stream.
 */
enum class StreamType
{
    VIDEO_4_LINUX,
    UNDEFINED
    // TODO : to be extended
};

/*!
 * Stores the input stream type and parameters.
 */
class StreamDescriptor
{
public:
    //! Constructor.
    StreamDescriptor(StreamType streamType = StreamType::UNDEFINED, QString parameters = ""):
        _streamType(streamType),
        _parameters(parameters)
    {}

    //! Convenience constructor, takes the stream type as as string.
    StreamDescriptor(QString streamType, QString parameters) :
        _streamType(StreamType::UNDEFINED)
    {
        if (_streamTypeByName.contains(streamType)){
            _streamType = _streamTypeByName[streamType];
            _parameters = parameters;
        }
    }

    //! Getter for the stream type.
    StreamType streamType() const { return _streamType; }
    //! Getter for the parameters.
    QString parameters() const { return _parameters; }
    //! Checks that the stream descriptor is valid.
    bool isValid() const { return (_streamType != StreamType::UNDEFINED); }

    //! Checks that the provided string is a valid stream type.
    static bool isValidStreamType(QString streamType) { return _streamTypeByName.contains(streamType); }

private:
    //! The type of the incoming video stream.
    StreamType _streamType;
    //! Additional parameters of the stream.
    QString _parameters;
    //! The map to translate the string stream type to the corresponding enum.
    static const QMap<QString, StreamType> _streamTypeByName;
};

/*!
 * The alias for the stream parameters list.
 */
using StreamDescriptorList = QList<StreamDescriptor>;

/*!
 * Class-signleton that is used to store parameters of the grabber; at the moment those are just input streams types and attributes.
 * Their values are received from the command line arguments.
 * NOTE : in C++11 there is no need for manual locking. Concurrent execution shall wait if a static local variable is already being initialized.
 * Based on http://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-singleton-in-c11-without-using-mutex
 */
class CommandLineParameters
{
public:
    //! The singleton getter.
    static CommandLineParameters& get();

    //! Initializes the parameters from the command line arguments. Returns true if at least the
    //! main camera parameter is found and there were no "-h" option.
    //! It receives also flags difining which command line parameters are considired essential
    //! so that the initialisation passes.
    bool init(int argc, char** argv, bool needMainCamera = true);

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
    //! Returns the parameters of the main camera.
    StreamDescriptor mainCameraDescriptor() const { return _mainCameraDescriptor; }

private:
    //! Constructor. Defining it here prevents construction.
    CommandLineParameters();
    //! Destructor. Defining it here prevents unwanted destruction.
    ~CommandLineParameters();

private:
    //! Prints out the command line arguments expected by the video grabber.
    void printSupportedArguments();
    //! Looks for the given argument in the command line, if found it return second and third argument as
    //! stream type and parameters.
    bool parseStreamArguments(int argc, char** argv, QString argument, QString& streamType, QString& parameters);

private:
    // TODO : refactor to integrate SetupType based more generic approach
    //! Input streams parameters.
    StreamDescriptor _mainCameraDescriptor;
};

#endif // CATS2_COMMAND_LINE_PARAMETERS_HPP
