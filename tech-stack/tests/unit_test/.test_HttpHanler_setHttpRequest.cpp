// test_HttpHandler.cpp
#include "catch.hpp"
#include <request/HttpHandler.hpp>
#include <vector>
#include <utility> // pour std::pair
#include <sstream>

// Helper pour créer une requête HTTP brute (version C++98)
std::string create_raw_request(
    const std::string& method,
    const std::string& path,
    const std::string& version,
    const std::vector<std::pair<std::string, std::string> >& headers,
    const std::string& body = ""
) {
    std::ostringstream request;
    request << method << " " << path << " " << version << "\r\n";
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        request << it->first << ": " << it->second << "\r\n";
    }
    request << "\r\n" << body;
    return request.str();
}

TEST_CASE("setHttpRequest parses valid HTTP requests", "[setHttpRequest]") {
    SECTION("Valid GET request") {
        std::vector<std::pair<std::string, std::string> > headers;
        headers.push_back(std::make_pair("Host", "example.com"));
        headers.push_back(std::make_pair("User-Agent", "Test"));
        std::string raw_request = create_raw_request(
            "GET", "/path", "HTTP/1.1", headers, "Body content"
        );
        t_httpRequest request = HttpHandler::setHttpRequest(raw_request);
        REQUIRE(request.method == "GET");
        REQUIRE(request.path == "/path");
        REQUIRE(request.version == "HTTP/1.1");
        REQUIRE(request.headers["Host"] == "example.com");
        REQUIRE(request.headers["User-Agent"] == "Test");
        REQUIRE(request.body == "Body content");
    }

    SECTION("Valid POST request") {
        std::vector<std::pair<std::string, std::string> > headers;
        headers.push_back(std::make_pair("Host", "example.com"));
        headers.push_back(std::make_pair("Content-Type", "application/json"));
        std::string raw_request = create_raw_request(
            "POST", "/submit", "HTTP/1.1", headers, "{\"key\":\"value\"}"
        );
        t_httpRequest request = HttpHandler::setHttpRequest(raw_request);
        REQUIRE(request.method == "POST");
        REQUIRE(request.path == "/submit");
        REQUIRE(request.version == "HTTP/1.1");
        REQUIRE(request.headers["Content-Type"] == "application/json");
        REQUIRE(request.body == "{\"key\":\"value\"}");
    }
}

TEST_CASE("setHttpRequest handles invalid HTTP requests", "[setHttpRequest]") {
    SECTION("Invalid method") {
        std::vector<std::pair<std::string, std::string> > headers;
        headers.push_back(std::make_pair("Host", "example.com"));
        std::string raw_request = create_raw_request(
            "INVALID", "/path", "HTTP/1.1", headers
        );
        t_httpRequest request = HttpHandler::setHttpRequest(raw_request);
        REQUIRE(request.method != "INVALID");
    }

    SECTION("Missing path") {
        std::vector<std::pair<std::string, std::string> > headers;
        headers.push_back(std::make_pair("Host", "example.com"));
        std::string raw_request = create_raw_request(
            "GET", "", "HTTP/1.1", headers
        );
        t_httpRequest request = HttpHandler::setHttpRequest(raw_request);
        REQUIRE(request.path.empty());
    }

    SECTION("Malformed request line") {
        std::string raw_request = "GET /path";
        t_httpRequest request = HttpHandler::setHttpRequest(raw_request);
        bool isInvalid = request.method.empty() || request.version.empty();
        REQUIRE(isInvalid == true);
    }
}

