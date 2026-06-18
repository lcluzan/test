// test_setHttpResponse.cpp
#include "catch.hpp"
#include <request/HttpHandler.hpp>
#include <map>

// Helper pour créer une t_httpRequest
t_httpRequest create_request(
    const std::string& method,
    const std::string& path,
    const std::map<std::string, std::string>& headers = std::map<std::string, std::string>(),
    const std::string& body = ""
) {
    t_httpRequest request;
    request.method = method;
    request.path = path;
    request.headers = headers;
    request.body = body;
    return request;
}

TEST_CASE("setHttpResponse generates correct HTTP responses", "[setHttpResponse]") {
    SECTION("Valid GET request for static file") {
        t_httpRequest request = create_request("GET", "/index.html");
        t_httpResponse response = HttpHandler::setHttpResponse(request);
        REQUIRE(response.status == 200); // Supposons que serveStaticFile retourne 200 pour un fichier existant
        REQUIRE(response.headers["Content-Type"] == "text/html");
        REQUIRE(!response.body.empty());
    }

    SECTION("Valid GET request for root path") {
        t_httpRequest request = create_request("GET", "/");
        t_httpResponse response = HttpHandler::setHttpResponse(request);
        REQUIRE(response.status == 200);
        REQUIRE(response.headers["Content-Type"] == "text/html");
    }

    SECTION("Invalid HTTP method") {
        t_httpRequest request = create_request("INVALID", "/path");
        t_httpResponse response = HttpHandler::setHttpResponse(request);
        REQUIRE(response.status == 400);
        REQUIRE(response.headers["Content-Type"] == "text/html");
        REQUIRE(response.body.find("400 Bad Request") != std::string::npos);
    }

    SECTION("Non-existent static file") {
        t_httpRequest request = create_request("GET", "/nonexistent.html");
        t_httpResponse response = HttpHandler::setHttpResponse(request);
        REQUIRE(response.status == 404);
        REQUIRE(response.headers["Content-Type"] == "text/html");
        REQUIRE(response.body.find("404 Not Found") != std::string::npos);
    }

    SECTION("CGI request (not implemented)") {
        t_httpRequest request = create_request("GET", "/cgi-bin/script.cgi");
        t_httpResponse response = HttpHandler::setHttpResponse(request);
        // Ici, on s'attend à ce que la réponse indique que le CGI n'est pas implémenté
        // (selon ton code, cela pourrait être une réponse 501 ou un message de log)
        REQUIRE(response.status == 501); // À adapter selon ton implémentation
    }
}
