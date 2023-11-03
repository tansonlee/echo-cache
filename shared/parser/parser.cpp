#include "parser.h"
#include <stdexcept>

StatusCode intToStatusCode(int code) {
    switch (code) {
        case 0:
            return StatusCode::success;
        case 1:
            return StatusCode::parsingFailure;
        case 2:
            return StatusCode::keyNotFound;
        case 3:
            return StatusCode::invalidCommand;
        case 4:
            return StatusCode::unexpectedError;
        default:
            throw std::invalid_argument("No such status code: " + std::to_string(code));
    }
}

int find_separator(std::string str, int start) {
    // Find the separator repeated twice.
    size_t length = str.length();
    for (size_t i = start; i < length - 1; ++i) {
        if (str[i] == COMMAND_SEPARATOR && str[i + 1] == COMMAND_SEPARATOR) {
            return i;
        }
    }

    return -1;
}

CommandType getCommandType(std::string command) {
    int separator_index = find_separator(command, 0);

    if (separator_index == -1) {
        return CommandType::other;
    }

    std::string commandTypeString = command.substr(0, separator_index);
    if (commandTypeString == "get") {
        return CommandType::get;
    }
    if (commandTypeString == "del") {
        return CommandType::del;
    }
    if (commandTypeString == "set") {
        return CommandType::set;
    }

    return CommandType::other;
}

ParsedSetRequest parseSet(std::string command) {
    int first_separator_index = find_separator(command, 0);
    if (first_separator_index == -1) {
        return {false, "", ""};
    }
    int key_start = first_separator_index + 2;

    int second_separator_index = find_separator(command, key_start);
    if (second_separator_index  == -1) {
        return {false, "", ""};
    }

    int key_end = second_separator_index - 1;
    int key_length = key_end - key_start + 1;

    int val_start = second_separator_index + 2;
    int val_end = command.length() - 1;
    int val_length = val_end - val_start + 1;

    return {true, command.substr(key_start, key_length), command.substr(val_start, val_length)};
}

ParsedGetRequest parseGet(std::string command) {
    int separator_index = find_separator(command, 0);

    if (separator_index == -1) {
        return {false, ""};
    }

    int key_start = separator_index + 2;
    int key_end = command.length() - 1;
    int key_length = key_end - key_start + 1;

    return {true, command.substr(key_start, key_length)};
}

ParsedDelRequest parseDel(std::string command) {
    int separator_index = find_separator(command, 0);

    if (separator_index == -1) {
        return {false, ""};
    }

    int key_start = separator_index + 2;
    int key_end = command.length() - 1;
    int key_length = key_end - key_start + 1;

    return {true, command.substr(key_start, key_length)};
}

ParsedKey parseKey(std::string command) {
    CommandType commandType = getCommandType(command);

    if (commandType == CommandType::get) {
        ParsedGetRequest getRequest = parseGet(command);
        if (getRequest.success) {
            return {true, getRequest.key};
        }
    }
    if (commandType == CommandType::del) {
        ParsedDelRequest delRequest = parseDel(command);
        if (delRequest.success) {
            return {true, delRequest.key};
        }
    }
    if (commandType == CommandType::set) {
        ParsedSetRequest setRequest = parseSet(command);
        if (setRequest.success) {
            return {true, setRequest.key};
        }
    }

    return {false, ""};
}

std::string formatResponseString(HandlerResponse response) {
    return std::to_string(response.statusCode) + "||" + response.result;
}

HandlerResponse parseResponseString(std::string response) {
    int separator_index = find_separator(response, 0);

    StatusCode statusCode = intToStatusCode(stoi(response.substr(0, separator_index)));

    int response_start = separator_index + 2;
    int response_end = response.length() - 1;
    int response_length = response_end - response_start + 1;
    std::string result = response.substr(separator_index + 2, response_length);

    return { statusCode, result };
}