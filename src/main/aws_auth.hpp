#pragma once

#include <string>

#include "cpprest/http_client.h"
#include <boost/date_time/posix_time/posix_time.hpp>

class Auth {
public:
	Auth(const std::string accessKeyId, const std::string secretAccessKey, const bool verbose = false);
	~Auth() {};

	std::string base64(const unsigned char * data, const unsigned int dataLength);
	std::string hex(const unsigned char * data, const unsigned int dataLength);
	std::string hash(const std::string input, const std::string method = "SHA256");
	std::string createCanonicalRequest(web::http::http_request httpRequest, const bool signedPayload);
	std::string createSigningKey(std::string date, std::string awsRegion, std::string awsService, unsigned char * signingKey, unsigned int * kSigning_len);
	std::string createStringToSign(std::string date, std::string iso_date, std::string awsRegion, std::string awsService, std::string canonicalHttpRequestHash);
	std::string createSignature(const std::string stringToSign, const unsigned char * signingKey, const unsigned int kSigning_len);
	web::http::http_request signRequestInQuery(const boost::posix_time::ptime time, web::http::http_request request, const std::string region, const std::string service, const bool signedPayload = true);
	web::http::http_request signRequestInHeaders(const boost::posix_time::ptime time, web::http::http_request request, const std::string region, const std::string service, const bool signedPayload = true);

private:
	const std::string accessKeyId;
	const std::string secretAccessKey;
	const bool verbose;
};


