#ifndef PARSER_H
#define PARSER_H

#include <string>

/*
 * Commands are expected to be formatted in the following way:
 *
 * ### Set command;
 * set||<key>||<value>
 * 
 * ### Get command:
 * get||<key>
 */

struct ParsedSetRequest {
    bool success;
    std::string key;
    std::string val;
};

struct ParsedGetRequest {
    bool success;
    std::string key;
};

struct ParsedKey {
    bool success;
    std::string key;
};

enum CommandType {
    other = 0,
    set = 1,
    get = 2,
};

enum StatusCode {
    success = 0,
    parsingFailure = 1,
    keyNotFound = 2,
    invalidCommand = 3,
    unexpectedError = 4
};

struct HandlerResponse {
    StatusCode statusCode;
    std::string result;
};

const char COMMAND_SEPARATOR = '|';

// set||name||tanson -> set
// get||name -> get
CommandType getCommandType(std::string command);

// set||name||tanson -> { true, name, tanson}
ParsedSetRequest parseSet(std::string command);

// get||name -> { true, name }
ParsedGetRequest parseGet(std::string command);

// set||name||tanson -> name
// get||name -> name
ParsedKey parseKey(std::string command);

// { 0, "tanson" } -> 0||tanson
std::string formatResponseString(HandlerResponse response);
HandlerResponse parseResponseString(std::string response);

#endif
