
#include "ParseCloud.h"
#include "User/ParseUser.h"
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <catch2/catch.hpp>

using namespace std;
using namespace parsecloud;


TEST_CASE("ParseUser SingUp", "[SingUp]") {
    ParseCloud::setApplicationConfig(SERVER_URL, APPID, APPKEY);

    ParseUser *user = ParseUser::user();
    user->username = "myname";
    user->password = "123456";
    user->email = "myname@example.com";

    user->signUp()
            .then([](PCError error) {
                CHECK(error.code == 0);
            }).then([user]() {
                CHECK(user->isAuthenticated());
                return user->deleteInBackgroundWithCallback();
            })
            .then([](PCError error) {
                CHECK(error.code == 0);
            }).then([user]() {
                ParseUser::logOut();
                user->release();
            });


}

TEST_CASE("ParseUser Login", "[Login]") {
    ParseCloud::setApplicationConfig(SERVER_URL, APPID,
                                     APPKEY);
    auto user = ParseUser::user();
    user->username = "myname";
    user->password = "123456";
    user->email = "myname@example.com";
    user->signUp()
            .then([](PCError error) {
                CHECK(error.code == 0);
                ParseUser::logOut();
                CHECK(ParseUser::loginWithUsernameAndPassword("myname", "123456") != nullptr);
                REQUIRE (ParseUser::currentUser() != nullptr);
                CHECK (ParseUser::currentUser()->isAuthenticated());
                return ParseUser::currentUser()->deleteInBackgroundWithCallback();
            }).
                    then([user](PCError error) {
                CHECK(error.code == 0);
                user->release();
            });
}

TEST_CASE("ParseUser UpdatePassowrd", "[Password]") {
    ParseCloud::setApplicationConfig(SERVER_URL, APPID, APPKEY);

    ParseUser *user = ParseUser::user();
    user->username = "myname";
    user->password = "123456";
    user->email = "myname@example.com";
    user->signUp();

    user->updatePasswordWithCallback("123456", "654321")
    .then([](PCError error){
        CHECK(error.code == 0);
        ParseUser::logOut();
        CHECK(ParseUser::loginWithUsernameAndPassword("myname", "654321") !=nullptr);
        REQUIRE (ParseUser::currentUser() != nullptr);
        CHECK (ParseUser::currentUser()->isAuthenticated());
        return ParseUser::currentUser()->deleteInBackgroundWithCallback();
    }).then([user](PCError error){
        CHECK(error.code == 0);
        user->release();
    });
}

//!!TODO need configured server to use this
TEST_CASE("RequestPasswordResetForEmail", "[Password]") {
    ParseCloud::setApplicationConfig(SERVER_URL, APPID,
                                     APPKEY);

    ParseUser *user = ParseUser::user();
    user->username = "myname";
    user->password = "123456";
    user->email = "myname@example.com";
    user->signUp();

    REQUIRE (ParseUser::currentUser() != nullptr);

    CHECK (ParseUser::currentUser()->isAuthenticated());

    ParseUser::requestPasswordResetForEmail("myname@example.com")
    .then([user](PCError error){
        CHECK(error.code == 0);
        return user->deleteInBackgroundWithCallback();

    }).then([user](PCError error){
        CHECK(error.code == 0);
        user->release();
    });

}
