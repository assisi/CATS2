#ifndef CATS2_COMMAND_LINE_PARSER_HPP
#define CATS2_COMMAND_LINE_PARSER_HPP

#include <QtCore/QString>

/*!
 * Reads the command line arguments.
 */
class CommandLineParser
{
public:
    //! Looks for the given argument in the command line, if found it return
    //! second and third argument as type and parameters.
    static bool parseTypedArgument(int argc, char** argv,
                                   QString argument,
                                   QString& argumentType,
                                   QString& parameters);
    //! Looks for the given argument in the command line, if found it return the
    //! argument as configuration file name.
    static bool parseArgument(int argc, char** argv,
                              QString argument,
                              QString& argumentValue);
};

#endif // CATS2_COMMAND_LINE_PARSER_HPP
