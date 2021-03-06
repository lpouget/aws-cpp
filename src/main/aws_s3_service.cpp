#include <vector>
#include <string>
#include <fstream>

#include <boost/algorithm/string/replace.hpp>
#include "pplx/pplxtasks.h"
#include <cpprest/http_msg.h>

#include "aws_s3_service.hpp"

AwsS3Service::AwsS3Service(Auth auth)
	: auth(auth){

}

AwsResponse<AwsS3Object> AwsS3Service::getObject(std::string bucketName, std::string objectName) {
	boost::posix_time::ptime time = boost::posix_time::second_clock::universal_time();
	web::http::http_request http_request;
	web::http::uri_builder uri_builder;
	std::string message;

	const std::string region = "eu-west-1";
	const std::string service = "s3";

	uri_builder.set_path("/" + objectName);

	std::string host = bucketName + ".s3.amazonaws.com:80";
	std::string uri;

	uri.append("http://").append(host).append("/");

	web::http::client::http_client http_client(uri.c_str());

	http_request.set_method(web::http::methods::GET);
	http_request.set_request_uri(uri_builder.to_uri());
	http_request.headers().add<std::string>("host", host);

	http_request = this->auth.signRequestInQuery(time, http_request, region, service, false);

	http_request.headers().remove("host");

	web::http::http_response response = http_client.request(http_request).get();
// 	std::vector<unsigned char>

	if(response.status_code() == web::http::status_codes::OK) {
		return AwsResponse<AwsS3Object>(response.status_code(), "", AwsS3Object(response.extract_vector().get()));
	}

	return AwsResponse<AwsS3Object>(response.status_code(), response.extract_string().get(), AwsS3Object());
}

void AwsS3Service::putObject(std::string bucketName, std::string objectName, std::vector<unsigned char> data) {
	std::ostringstream oss;

	for(unsigned char c : data) {
		oss << c;
	}

	putObject(bucketName, objectName, oss.str());
}

void AwsS3Service::putObject(std::string bucketName, std::string objectName, std::string data) {
	boost::posix_time::ptime time = boost::posix_time::second_clock::universal_time();
	web::http::http_request http_request;
	web::http::uri_builder uri_builder;
	std::string message;

	const std::string region = "eu-west-1";
	const std::string service = "s3";

	uri_builder.set_path("/" + objectName);

	std::string host = bucketName + ".s3.amazonaws.com:80";
	std::string uri;

	uri.append("http://").append(host).append("/");

	web::http::client::http_client http_client(uri.c_str());

	http_request.set_method(web::http::methods::PUT);
	http_request.set_request_uri(uri_builder.to_uri());
// 	http_request.headers().add<std::string>("Content-MD5", auth.hash(data, "MD5"));
	http_request.headers().add<std::string>("host", host);
	http_request.set_body(data);

	Concurrency::streams::streambuf<uint8_t> target();

	http_request = auth.signRequestInQuery(time, http_request, region, service, false);
	http_request.headers().remove("host");

	web::http::http_response response = http_client.request(http_request).then([=](web::http::http_response response){
		return response.extract_string();
	}).then([=](utility::string_t str){
		//TODO handle return message
		//std::cout << str << std::endl;
	}).wait();
}
