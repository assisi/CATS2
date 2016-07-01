#ifndef VIDEO_GRABBER_SETTINGS_HPP
#define VIDEO_GRABBER_SETTINGS_HPP

#include "StreamReceiver.hpp"

/*!
 * Class-signleton that is used to store parameters of the grabber; at the moment those are just input streams types and attributes.
 * Their values are received from the command line arguments.
 * Based on http://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-singleton-in-c11-without-using-mutex
 */
class VideoGrabberSettings
{
public:
    //! The singleton getter.
    static VideoGrabberSettings& get();

    //! Initializes the parameters from the command line arguments. Returns true if at least the
    //! main camera parameter is found and there were no "-h" option.
    bool init(int argc, char** argv);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    VideoGrabberSettings(VideoGrabberSettings const&) = delete;
    //! Move constructor.
    VideoGrabberSettings(VideoGrabberSettings&&) = delete;
    //! Copy assignment.
    VideoGrabberSettings& operator=(VideoGrabberSettings const&) = delete;
    //! Move assignment.
    VideoGrabberSettings& operator=(VideoGrabberSettings &&) = delete;

public:
    //! Returns the parameters of the main camera.
    StreamDescriptor mainCameraDescriptor() { return _mainCameraDescriptor; }

private:
    //! Constructor. Defining it here prevents construction.
    VideoGrabberSettings();
    //! Destructor. Defining it here prevents unwanted destruction.
    ~VideoGrabberSettings();

private:
    //! Prints out the command line arguments expected by the video grabber.
    void printSupportedArguments();
    //! Looks for the given argument in the command line, if found it return second and third argument as
    //! stream type and parameters.
    bool parseStreamArguments(int argc, char** argv, QString argument, QString& streamType, QString& parameters);

private:
    //! Input streams parameters.
    StreamDescriptor _mainCameraDescriptor;
};

#endif // VIDEO_GRABBER_SETTINGS_HPP
