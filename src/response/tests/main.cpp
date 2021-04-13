//
// Created by Gueren Drive on 4/12/21.
//

#include "gtest/gtest.h"
#include "response.hpp"

TEST(StatusCodes, Messages) {
  Response continue_code(Response::StatusCodes::kContinue);
  ASSERT_STREQ("100 Continue", continue_code.get_status_message().c_str());

  Response processing(Response::StatusCodes::kProcessing);
  ASSERT_STREQ("102 Processing", processing.get_status_message().c_str());

  Response ok(Response::StatusCodes::kOk);
  ASSERT_STREQ("200 OK", ok.get_status_message().c_str());

  Response created(Response::StatusCodes::kCreated);
  ASSERT_STREQ("201 Created", created.get_status_message().c_str());

  Response accepted(Response::StatusCodes::kAccepted);
  ASSERT_STREQ("202 Accepted", accepted.get_status_message().c_str());

  Response movedPermanently(Response::StatusCodes::kMovedPermanently);
  ASSERT_STREQ("301 Moved Permanently",
               movedPermanently.get_status_message().c_str());

  Response badRequest(Response::StatusCodes::kBadRequest);
  ASSERT_STREQ("400 Bad Request", badRequest.get_status_message().c_str());

  Response forbidden(Response::StatusCodes::kForbidden);
  ASSERT_STREQ("403 Forbidden", forbidden.get_status_message().c_str());

  Response notFound(Response::StatusCodes::kNotFound);
  ASSERT_STREQ("404 Not Found", notFound.get_status_message().c_str());

  Response methodNotAllowed(Response::StatusCodes::kMethodNotAllowed);
  ASSERT_STREQ("405 Method Not Allowed",
               methodNotAllowed.get_status_message().c_str());

  Response requestTimeout(Response::StatusCodes::kRequestTimeout);
  ASSERT_STREQ("408 Request Timeout",
               requestTimeout.get_status_message().c_str());

  Response lengthRequired(Response::StatusCodes::kLengthRequired);
  ASSERT_STREQ("411 Length Required",
               lengthRequired.get_status_message().c_str());

  Response payloadTooLarge(Response::StatusCodes::kPayloadTooLarge);
  ASSERT_STREQ("413 Payload Too Large",
               payloadTooLarge.get_status_message().c_str());

  Response internalServerError(Response::StatusCodes::kInternalServerError);
  ASSERT_STREQ("500 Internal Server Error",
               internalServerError.get_status_message().c_str());
}

TEST(Headers, AddHeader) {
  Response mouse(Response::StatusCodes::kOk);

  mouse.AddHeader("Host", "www.glebhomevideos.com");
  ASSERT_STREQ("www.glebhomevideos.com",
               mouse.get_header_value("Host").c_str());
}

TEST(Body, AddToBody) {
  Response mouse(Response::StatusCodes::kOk);

  mouse.AddToBody("mgeneviv and gdrive are monkes");
  ASSERT_STREQ("mgeneviv and gdrive are monkes", mouse.get_body().c_str());
}
