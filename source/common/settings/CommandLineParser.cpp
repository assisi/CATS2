#include "CommandLineParser.hpp"

/*!
 * Looks for the given argument in the command line, if found it return second
 * and third argument as type and parameters.
 * This code is inspired by this stackoverflow post
 * http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
 */
bool CommandLineParser::parseTypedArgument(int argc, char** argv,
                                               QString argument,
                                               QString& argumentType,
                                               QString& parameters)
{
    char** itr = std::find(argv, argv + argc, argument);
    if ((itr != (argv + argc)) && ((itr + 1) != (argv + argc)) &&
            ((itr + 2) != (argv + argc)))
    {
        QString value = QString(*(itr + 1)).toLower();
        argumentType = value;
        parameters = *(itr + 2);
        return true;
    }
    return false;
}

/*!
 * Looks for the given argument in the command line, if found it return the
 * argument as configuration file name.
 */
bool CommandLineParser::parseArgument(int argc, char** argv,
                                      QString argument,
                                      QString& argumentValue)
{
    char** end = argv + argc;
    char** itr = std::find(argv, end, argument);
    if ((itr != end) && (++itr != end)) {
        argumentValue = QString(*itr);
        return true;
    }
    return false;
}
