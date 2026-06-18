
#include "../include/webserver_test.hpp" 

std::string typeToString(t_type type)
{
    switch (type) {
        case METHOD: return "METHOD";
        case PATH: return "PATH";
        case VERSION: return "VERSION";
        case HEADER_KEY: return "HEADER_KEY";
        case HEADER_VALUE: return "HEADER_VALUE";
        case COLON: return "COLON";
        case CRLF: return "CRLF";
        case BODY: return "BODY";
        case WHITESPACE: return "WHITESPACE";
        case UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

printTokens(const std::vector<t_token>& lexer)
{
    for (std::vector<t_token>::const_iterator it = lexer.begin(); it != lexer.end(); ++it)
    {
        const t_token& token = *it;
        std::cout << "Token type: " << typeToString(token.type) << ", value: \"" << token.value << "\"" << std::endl;
    }
}

print_hash(const std::vector<std::string>& hash)
{
    for (size_t i = 0; i < hash.size(); ++i)
    {
        std::cout << "Line " << i << ": \"" << hash[i] << "\"" << std::endl;
    }
}

/* =========================================================================== */
