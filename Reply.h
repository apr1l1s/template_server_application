#pragma once
#include "ErrorHandler.h"
#include <vector>
#include <nlohmann/json.hpp>
#include <chrono>
#include <string>
#include <boost/asio/buffer.hpp>


class Reply {
    enum content_type {
        html, json
    };
public:
    ErrorHandler::StatusType status;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string content;

    std::string get_date()
    {
        std::time_t localtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char date[50];
        struct tm timeinfo;
        localtime_s(&timeinfo, &localtime);
        strftime(date, sizeof date, "%a, %d %b %Y %T GMT", &timeinfo);
        return date;
    }

    Reply(ErrorHandler::StatusType _status, std::string _content = "") :status(_status) {
        headers.resize(7);

        headers[0].first = "Date: ";
        headers[1].first = "Server: ";
        headers[2].first = "Content-Type: ";
        headers[3].first = "Content-Length: ";
        headers[4].first = "Access-Control-Allow-Methods: ";
        headers[5].first = "Access-Control-Allow-Headers: ";
        headers[6].first = "Access-Control-Allow-Origin: ";

        headers[0].second = get_date();
        headers[1].second = "apr1l1s";
        if (status == ErrorHandler::StatusType::ok) {
            content = _content;
            headers[2].second = content_type_to_string(content_type::json);
        }
        else {
            nlohmann::json j;
            j["status"] = content_to_string(status);
            content = j.dump();
            headers[2].second = content_type_to_string(content_type::json);
        }
        headers[3].second = std::to_string(content.length());
        headers[4].second = "POST, OPTIONS, GET, PUT, DELETE";
        headers[5].second = "*";
        headers[6].second = "*";

    }

    std::string content_type_to_string(content_type type) {
        switch (type) {
        case html:
            return "text/html";
        case json:
            return "application/json";
        default:
            return "text/html";
        }
    }
    std::string content_to_string(ErrorHandler::StatusType status)
    {
        switch (status)
        {
        case ErrorHandler::StatusType::ok:
            return "";
        case ErrorHandler::StatusType::accepted:
            return "202 Accepted";
        case ErrorHandler::StatusType::no_content:
            return "204 Content";
        case ErrorHandler::StatusType::bad_request:
            return "400 Bad Request";
        case ErrorHandler::StatusType::unauthorized:
            return "401 Unauthorized";
        case ErrorHandler::StatusType::not_found:
            return "404 Not Found";
        case ErrorHandler::StatusType::internal_server_error:
            return "500 Internal Server Error>";
        default:
            return "500 Internal Server Error";
        }
    }
    std::string status_to_string(ErrorHandler::StatusType status) {
        switch (status)
        {
        case ErrorHandler::StatusType::ok:
            return "HTTP/1.0 200 OK\n";
        case ErrorHandler::StatusType::accepted:
            return "HTTP/1.0 202 Accepted\n";
        case ErrorHandler::StatusType::no_content:
            return "HTTP/1.0 204 No Content\n";
        case ErrorHandler::StatusType::bad_request:
            return "HTTP/1.0 400 Bad Request\n";
        case ErrorHandler::StatusType::unauthorized:
            return "HTTP/1.0 401 Unauthorized\n";
        case ErrorHandler::StatusType::not_found:
            return "HTTP/1.0 404 Not Found\n";
        case ErrorHandler::StatusType::internal_server_error:
            return "HTTP/1.0 500 Internal Server Error\n";
        default:
            return "HTTP/1.0 500 Internal Server Error\n";
        }
    }
    std::string to_string()
    {
        std::string crlf = "\n";
        std::string message;
        message += status_to_string(status);
        for (std::size_t i = 0; i < headers.size(); ++i)
        {
            std::pair<std::string, std::string>& h = headers[i];
            message += h.first + h.second + crlf;
        }
        message += crlf + content;
        return message;
    }
        
};

