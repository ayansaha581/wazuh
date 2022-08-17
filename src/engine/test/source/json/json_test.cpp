#include <json/json.hpp>
#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <limits>

#define GTEST_COUT std::cerr << "[          ] [ INFO ] "

using namespace std;
using namespace json;

TEST(JsonBase, InitializeDefault)
{
    ASSERT_NO_THROW(Json doc;);
    ASSERT_NO_THROW(Json doc {};);
}

TEST(JsonBase, InitializeCopy)
{
    Json doc;
    ASSERT_NO_THROW(Json doc2 {doc};);
}

TEST(JsonBase, AssignmentCopy)
{
    Json doc;
    ASSERT_NO_THROW(Json doc2 = doc;);
}

TEST(JsonBase, InitializeJsonString)
{
    ASSERT_NO_THROW(Json doc {"{\"key\":\"value\"}"};);
    ASSERT_NO_THROW(Json doc {"{}"};);
    ASSERT_THROW(Json doc {"{\"key\":\"value\"}}"};, std::runtime_error);
}

// TODO: Add more use cases, and add cases once operators and arrays are implemented.
TEST(JsonStatic, FormatJsonPath)
{
    auto dotPath = "key.value";
    std::string pointerPath;
    ASSERT_NO_THROW(pointerPath = Json::formatJsonPath(dotPath););
    ASSERT_EQ(pointerPath, "/key/value");

    dotPath = ".key.value";
    ASSERT_NO_THROW(pointerPath = Json::formatJsonPath(dotPath););
    ASSERT_EQ(pointerPath, "/key/value");
}

TEST(JsonBuildtime, Size)
{
    // Empty object
    Json emptyObj {"{}"};
    ASSERT_EQ(emptyObj.size(), 0);

    // Empty array
    Json emptyArr {"[]"};
    ASSERT_EQ(emptyArr.size(), 0);

    // Object
    Json obj {"{\"key\":\"value\"}"};
    ASSERT_EQ(obj.size(), 1);

    // Array
    Json arr {"[\"value\"]"};
    ASSERT_EQ(arr.size(), 1);

    // None object or array
    Json none {"null"};
    ASSERT_THROW(none.size(), std::runtime_error);
}

TEST(JsonBuildtime, Null)
{
    Json none {"null"};
    ASSERT_TRUE(none.isNull());
}

TEST(JsonBuildtime, Bool)
{
    Json trueVal {"true"};
    ASSERT_TRUE(trueVal.isBool());
    ASSERT_TRUE(trueVal.getBool().value());

    Json falseVal {"false"};
    ASSERT_TRUE(falseVal.isBool());
    ASSERT_FALSE(falseVal.getBool().value());
}

TEST(JsonBuildtime, Number)
{
    Json integer {"123"};
    ASSERT_TRUE(integer.isNumber());
    ASSERT_EQ(integer.getInt(), 123);

    Json real {"123.456"};
    ASSERT_TRUE(real.isNumber());
    ASSERT_EQ(real.getDouble(), 123.456);
}

TEST(JsonBuildtime, String)
{
    Json str {"\"value\""};
    ASSERT_TRUE(str.isString());
    ASSERT_EQ(str.getString(), "value");
}

TEST(JsonBuildtime, Array)
{
    Json arr {"[\"value\"]"};
    ASSERT_TRUE(arr.isArray());
    ASSERT_EQ(arr.size(), 1);
    ASSERT_EQ(arr.getArray().value()[0].getString().value(), "value");
}

TEST(JsonBuildtime, Object)
{
    Json obj {"{\"key\":\"value\"}"};
    ASSERT_TRUE(obj.isObject());
    ASSERT_EQ(obj.size(), 1);
    ASSERT_EQ(std::get<0>(obj.getObject().value()[0]), "key");
    ASSERT_EQ(std::get<1>(obj.getObject().value()[0]).getString(), "value");
}

TEST(JsonRuntime, InitializeCopyMove)
{
    Json doc;
    ASSERT_NO_THROW(Json doc2 {std::move(doc)};);
}

TEST(JsonRuntime, AssignmentCopyMove)
{
    Json doc;
    ASSERT_NO_THROW(Json doc2 = std::move(doc););
}

TEST(JsonRuntime, Exists)
{
    // One level deep
    Json doc {"{\"key\":\"value\"}"};
    ASSERT_TRUE(doc.exists("/key"));
    ASSERT_FALSE(doc.exists("/key2"));
    ASSERT_THROW(doc.exists("key"), std::runtime_error);
    ASSERT_THROW(doc.exists(".key"), std::runtime_error);

    // Two levels deep
    doc = Json {"{\"key\":{\"key2\":\"value\"}}"};
    ASSERT_TRUE(doc.exists("/key/key2"));
    ASSERT_FALSE(doc.exists("/key/key3"));
    ASSERT_THROW(doc.exists("key/key2/key3"), std::runtime_error);
    ASSERT_THROW(doc.exists(".key/key2/key3"), std::runtime_error);

    // Three levels deep
    doc = Json {"{\"key\":{\"key2\":{\"key3\":\"value\"}}}"};
    ASSERT_TRUE(doc.exists("/key/key2/key3"));
    ASSERT_FALSE(doc.exists("/key/key2/key4"));
    ASSERT_THROW(doc.exists("key/key2/key3/key4"), std::runtime_error);
    ASSERT_THROW(doc.exists(".key/key2/key3/key4"), std::runtime_error);
}

TEST(JsonRuntime, EqualsValue)
{
    Json doc {R"({
        "object": {
            "key": "value"
        },
        "array": [
            "value"
        ],
        "int": 123,
        "real": 123.456,
        "boolT": true,
        "boolF": false,
        "null": null,
        "string": "value",
        "nested": {
            "object": {
                "key": "value"
            },
            "array": [
                "value"
            ],
            "int": 123,
            "real": 123.456,
            "boolT": true,
            "boolF": false,
            "null": null,
            "string": "value"
        }
    })"};

    Json value;

    // Object
    value = Json {"{\"key\":\"value\"}"};
    ASSERT_TRUE(doc.equals("/object", value));
    ASSERT_TRUE(doc.equals("/nested/object", value));

    // Array
    value = Json {"[\"value\"]"};
    ASSERT_TRUE(doc.equals("/array", value));
    ASSERT_TRUE(doc.equals("/nested/array", value));

    // Integer
    value = Json {"123"};
    ASSERT_TRUE(doc.equals("/int", value));
    ASSERT_TRUE(doc.equals("/nested/int", value));

    // Real
    value = Json {"123.456"};
    ASSERT_TRUE(doc.equals("/real", value));
    ASSERT_TRUE(doc.equals("/nested/real", value));

    // Boolean
    value = Json {"true"};
    ASSERT_TRUE(doc.equals("/boolT", value));
    ASSERT_TRUE(doc.equals("/nested/boolT", value));

    value = Json {"false"};
    ASSERT_TRUE(doc.equals("/boolF", value));
    ASSERT_TRUE(doc.equals("/nested/boolF", value));

    // Null
    value = Json {"null"};
    ASSERT_TRUE(doc.equals("/null", value));
    ASSERT_TRUE(doc.equals("/nested/null", value));

    // String
    value = Json {"\"value\""};
    ASSERT_TRUE(doc.equals("/string", value));

    // Wrong pointer
    ASSERT_THROW(doc.equals("object/key", value), std::runtime_error);

    // Non-existent pointer
    ASSERT_FALSE(doc.equals("/non-existent", value));

    // False cases
    // TODO: Iterative comparation of all types
    value = Json {"\"value2\""};
    ASSERT_FALSE(doc.equals("/object", value));
    ASSERT_FALSE(doc.equals("/array", value));
    ASSERT_FALSE(doc.equals("/int", value));
    ASSERT_FALSE(doc.equals("/real", value));
    ASSERT_FALSE(doc.equals("/boolT", value));
    ASSERT_FALSE(doc.equals("/boolF", value));
    ASSERT_FALSE(doc.equals("/null", value));
    ASSERT_FALSE(doc.equals("/string", value));
    ASSERT_FALSE(doc.equals("/nested/object", value));
}

TEST(JsonRuntime, EqualsReference)
{
    Json doc {R"({
        "object": {
            "key": "value"
        },
        "array": [
            "value"
        ],
        "int": 123,
        "real": 123.456,
        "boolT": true,
        "boolF": false,
        "null": null,
        "string": "value",
        "nested": {
            "object": {
                "key": "value"
            },
            "array": [
                "value"
            ],
            "int": 123,
            "real": 123.456,
            "boolT": true,
            "boolF": false,
            "null": null,
            "string": "value"
        }
    })"};

    Json value;

    // Object
    ASSERT_TRUE(doc.equals("/object", "/nested/object"));
    ASSERT_TRUE(doc.equals("/nested/object", "/object"));

    // Array
    ASSERT_TRUE(doc.equals("/array", "/nested/array"));
    ASSERT_TRUE(doc.equals("/nested/array", "/array"));

    // Integer
    ASSERT_TRUE(doc.equals("/int", "/nested/int"));
    ASSERT_TRUE(doc.equals("/nested/int", "/int"));

    // Real
    ASSERT_TRUE(doc.equals("/real", "/nested/real"));
    ASSERT_TRUE(doc.equals("/nested/real", "/real"));

    // Boolean
    ASSERT_TRUE(doc.equals("/boolT", "/nested/boolT"));
    ASSERT_TRUE(doc.equals("/nested/boolT", "/boolT"));

    ASSERT_TRUE(doc.equals("/boolF", "/nested/boolF"));
    ASSERT_TRUE(doc.equals("/nested/boolF", "/boolF"));

    // Null
    ASSERT_TRUE(doc.equals("/null", "/nested/null"));
    ASSERT_TRUE(doc.equals("/nested/null", "/null"));

    // String
    ASSERT_TRUE(doc.equals("/string", "/nested/string"));
    ASSERT_TRUE(doc.equals("/nested/string", "/string"));

    // Wrong pointer
    ASSERT_THROW(doc.equals("object/key", "/nested/object"), std::runtime_error);
    ASSERT_THROW(doc.equals("/object", "object/key"), std::runtime_error);

    // Non-existent pointer
    ASSERT_FALSE(doc.equals("/nonexistent", "/nested/object"));
    ASSERT_FALSE(doc.equals("/nested/object", "/nonexistent"));

    // False cases
    // TODO: Iterative comparation of all types
    ASSERT_FALSE(doc.equals("/object", "/array"));
    ASSERT_FALSE(doc.equals("/array", "/object"));
    ASSERT_FALSE(doc.equals("/int", "/real"));
    ASSERT_FALSE(doc.equals("/real", "/int"));
    ASSERT_FALSE(doc.equals("/boolT", "/boolF"));
    ASSERT_FALSE(doc.equals("/boolF", "/boolT"));
    ASSERT_FALSE(doc.equals("/null", "/string"));
    ASSERT_FALSE(doc.equals("/string", "/null"));
    ASSERT_FALSE(doc.equals("/object", "/nested/int"));
    ASSERT_FALSE(doc.equals("/nested/int", "/object"));
}

TEST(JsonRuntime, SetValue)
{
    Json expected {R"({
        "object": {
        "key": "value"
        },
        "nested": {
            "object": {
                "key": "value"
            },
            "array": [
                "value"
            ],
            "int": 123,
            "real": 123.456,
            "boolT": true,
            "boolF": false,
            "null": null,
            "string": "value"
        },
        "array": [
            "value"
        ],
        "int": 123,
        "real": 123.456,
        "boolT": true,
        "boolF": false,
        "null": null,
        "string": "value"
    })"};
    Json doc {"{}"};

    // Object
    doc.set("/object", Json {"{\"key\": \"value\"}"});
    ASSERT_TRUE(doc.equals("/object", Json {"{\"key\": \"value\"}"}));
    doc.set("/nested/object", Json {"{\"key\": \"value\"}"});
    ASSERT_TRUE(doc.equals("/nested/object", Json {"{\"key\": \"value\"}"}));

    // Array
    doc.set("/array", Json {"[\"value\"]"});
    ASSERT_TRUE(doc.equals("/array", Json {"[\"value\"]"}));
    doc.set("/nested/array", Json {"[\"value\"]"});
    ASSERT_TRUE(doc.equals("/nested/array", Json {"[\"value\"]"}));

    // Integer
    doc.set("/int", Json {"123"});
    ASSERT_TRUE(doc.equals("/int", Json {"123"}));
    doc.set("/nested/int", Json {"123"});
    ASSERT_TRUE(doc.equals("/nested/int", Json {"123"}));

    // Real
    doc.set("/real", Json {"123.456"});
    ASSERT_TRUE(doc.equals("/real", Json {"123.456"}));
    doc.set("/nested/real", Json {"123.456"});
    ASSERT_TRUE(doc.equals("/nested/real", Json {"123.456"}));

    // Boolean
    doc.set("/boolT", Json {"true"});
    ASSERT_TRUE(doc.equals("/boolT", Json {"true"}));
    doc.set("/nested/boolT", Json {"true"});
    ASSERT_TRUE(doc.equals("/nested/boolT", Json {"true"}));

    doc.set("/boolF", Json {"false"});
    ASSERT_TRUE(doc.equals("/boolF", Json {"false"}));
    doc.set("/nested/boolF", Json {"false"});
    ASSERT_TRUE(doc.equals("/nested/boolF", Json {"false"}));

    // Null
    doc.set("/null", Json {"null"});
    ASSERT_TRUE(doc.equals("/null", Json {"null"}));
    doc.set("/nested/null", Json {"null"});
    ASSERT_TRUE(doc.equals("/nested/null", Json {"null"}));

    // String
    doc.set("/string", Json {"\"value\""});
    ASSERT_TRUE(doc.equals("/string", Json {"\"value\""}));
    doc.set("/nested/string", Json {"\"value\""});
    ASSERT_TRUE(doc.equals("/nested/string", Json {"\"value\""}));

    // Expected
    ASSERT_EQ(expected.str(), doc.str());

    // Wrong pointer
    ASSERT_THROW(doc.set("object/key", Json {"\"value\""}), std::runtime_error);
}

TEST(JsonRuntime, SetReference)
{
    Json doc1 {R"({
        "nested": {
            "object": {
                "key": "value"
            },
            "array": [
                "value"
            ],
            "int": 123,
            "real": 123.456,
            "boolT": true,
            "boolF": false,
            "null": null,
            "string": "value"
        }
    })"};

    Json doc2 {R"({
        "nested": {},
        "object": {
            "key": "value"
        },
        "array": [
            "value"
        ],
        "int": 123,
        "real": 123.456,
        "boolT": true,
        "boolF": false,
        "null": null,
        "string": "value"
    })"};

    // Object
    doc1.set("/object", "/nested/object");
    ASSERT_TRUE(doc1.equals("/object", "/nested/object"));
    doc2.set("/nested/object", "/object");
    ASSERT_TRUE(doc2.equals("/nested/object", "/object"));

    // Array
    doc1.set("/array", "/nested/array");
    ASSERT_TRUE(doc1.equals("/array", "/nested/array"));
    doc2.set("/nested/array", "/array");
    ASSERT_TRUE(doc2.equals("/nested/array", "/array"));

    // Integer
    doc1.set("/int", "/nested/int");
    ASSERT_TRUE(doc1.equals("/int", "/nested/int"));
    doc2.set("/nested/int", "/int");
    ASSERT_TRUE(doc2.equals("/nested/int", "/int"));

    // Real
    doc1.set("/real", "/nested/real");
    ASSERT_TRUE(doc1.equals("/real", "/nested/real"));
    doc2.set("/nested/real", "/real");
    ASSERT_TRUE(doc2.equals("/nested/real", "/real"));

    // Boolean
    doc1.set("/boolT", "/nested/boolT");
    ASSERT_TRUE(doc1.equals("/boolT", "/nested/boolT"));
    doc2.set("/nested/boolT", "/boolT");
    ASSERT_TRUE(doc2.equals("/nested/boolT", "/boolT"));

    doc1.set("/boolF", "/nested/boolF");
    ASSERT_TRUE(doc1.equals("/boolF", "/nested/boolF"));
    doc2.set("/nested/boolF", "/boolF");
    ASSERT_TRUE(doc2.equals("/nested/boolF", "/boolF"));

    // Null
    doc1.set("/null", "/nested/null");
    ASSERT_TRUE(doc1.equals("/null", "/nested/null"));
    doc2.set("/nested/null", "/null");
    ASSERT_TRUE(doc2.equals("/nested/null", "/null"));

    // String
    doc1.set("/string", "/nested/string");
    ASSERT_TRUE(doc1.equals("/string", "/nested/string"));
    doc2.set("/nested/string", "/string");
    ASSERT_TRUE(doc2.equals("/nested/string", "/string"));

    // Expected
    ASSERT_EQ(doc1.str(), doc2.str());

    // Wrong pointer
    ASSERT_THROW(doc1.set("object/key", "/nested/object"), std::runtime_error);
    ASSERT_THROW(doc1.set("/object", "object/key"), std::runtime_error);

    // Reference to non-existent object, maps to null
    doc1.set("/object", "/non-existent");
    ASSERT_TRUE(doc1.equals("/object", Json {"null"}));
}

TEST(JsonRuntime, PrettyStr)
{
    std::string expected = R"({
    "nested": {
        "object": {
            "key": "value"
        },
        "array": [
            "value"
        ],
        "int": 123,
        "real": 123.456,
        "boolT": true,
        "boolF": false,
        "null": null,
        "string": "value"
    }
})";

    Json doc {expected.c_str()};

    ASSERT_EQ(expected, doc.prettyStr());
}

TEST(JsonRuntime, Str)
{
    std::string expected = "{\"nested\":{\"object\":{\"key\":\"value\"},\"array\":["
                           "\"value\"],\"int\":123,\"real\":123.456,\"boolT\":true,"
                           "\"boolF\":false,\"null\":null,\"string\":\"value\"}}";

    Json doc {expected.c_str()};

    ASSERT_EQ(expected, doc.str());
}

// Checking basic functionality of str from path method
TEST(JsonRuntime, strFromPath)
{
    std::string expected =
        R"({
            "field": "value",
            "nested": {
                "object": {
                    "key": "value"
                },
                "array": [
                    "value1",
                    "value2",
                    "value3",
                    "value4"
                ],
                "int": 123,
                "float": 123.456,
                "boolT": true,
                "boolF": false,
                "null": null
            }
        })";

    Json doc {expected.c_str()};

    ASSERT_EQ(doc.str("/field"), "\"value\"");
    ASSERT_EQ(doc.str("/nested/object"), "{\"key\":\"value\"}");
    ASSERT_EQ(doc.str("/nested/array"), "[\"value1\",\"value2\",\"value3\",\"value4\"]");
    ASSERT_EQ(doc.str("/nested/int"), "123");
    ASSERT_EQ(doc.str("/nested/float"), "123.456");
    ASSERT_EQ(doc.str("/nested/boolT"), "true");
    ASSERT_EQ(doc.str("/nested/boolF"), "false");
    ASSERT_EQ(doc.str("/nested/null"), "null");
}

// Cheking that returns nullopt when no present but correct field format
TEST(JsonRuntime, strFromPathNotPresentField)
{
    std::string expected =
        R"({
            "NotSearchedField": "value",
            "nested":
            {
                "object": {"SearchedField": "value"},
                "array": ["SearchedField"]
            }
        })";

    Json doc {expected.c_str()};

    ASSERT_EQ(doc.str("/SearchedField"), std::nullopt);
    ASSERT_EQ(doc.str("/nested/object/SearchedField"), "\"value\"");
}

// Cheking that throws runtime_error when no valid pointer
TEST(JsonRuntime, strFromPathNotCorrectPointer)
{
    std::string expected =
        R"({
            "Field": "value"
        })";

    Json doc {expected.c_str()};

    ASSERT_THROW(doc.str("Field"), std::runtime_error);
    ASSERT_THROW(doc.str("-/Field"), std::runtime_error);
    ASSERT_THROW(doc.str("-Field"), std::runtime_error);
    ASSERT_EQ(doc.str("/Field"), "\"value\"");
}

//return various stages of nested objects
TEST(JsonRuntime, strFromPathNestedObjects)
{
    std::string expected =
        R"({
            "A":
            {
                "B":
                {
                    "C":
                    {
                        "D":
                        {
                            "key": "value"
                        }
                    }
                }
            }
        })";

    Json doc {expected.c_str()};

    ASSERT_EQ(doc.str("/A"), "{\"B\":{\"C\":{\"D\":{\"key\":\"value\"}}}}");
    ASSERT_EQ(doc.str("/A/B"), "{\"C\":{\"D\":{\"key\":\"value\"}}}");
    ASSERT_EQ(doc.str("/A/B/C"), "{\"D\":{\"key\":\"value\"}}");
    ASSERT_EQ(doc.str("/A/B/C/D"), "{\"key\":\"value\"}");
    ASSERT_EQ(doc.str("/D"), std::nullopt);
}

/****************************************************************************************/
// GETTERS
/****************************************************************************************/
TEST(JsonGettersTest, GetString)
{
    // Success cases
    Json jObjStr {R"({
        "nested": "value"
    })"};
    Json jStr {"\"value\""};
    std::optional<std::string> got;
    ASSERT_NO_THROW(got = jObjStr.getString("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ("value", got.value());
    ASSERT_NO_THROW(got = jStr.getString());
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ("value", got.value());

    // Failure cases
    std::vector<Json> failureCases = {Json {R"({
                "nested": 123
            })"},
                                      Json {"123"},
                                      Json {R"({
                "nested": 123.456
            })"},
                                      Json {"123.456"},
                                      Json {R"({
                "nested": true
            })"},
                                      Json {"true"},
                                      Json {R"({
                "nested": false
            })"},
                                      Json {"false"},
                                      Json {R"({
                "nested": null
            })"},
                                      Json {"null"},
                                      Json {R"({
                "nested": {
                    "key": "value"
                }
            })"},
                                      Json {R"({
                "key": "value"
            })"},
                                      Json {R"({
                "nested": [
                    "value"
                ]
            })"},
                                      Json {"[\"value\"]"}};

    for (auto i = 0; i < failureCases.size(); i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_NO_THROW(got = failureCases[i].getString("/nested"));
            ASSERT_FALSE(got.has_value());
        }
        else
        {
            ASSERT_NO_THROW(got = failureCases[i].getString());
            ASSERT_FALSE(got.has_value());
        }
    }

    // Wrong pointer
    ASSERT_THROW(jObjStr.getString("object/key"), std::runtime_error);
}

TEST(JsonGettersTest, GetInt)
{
    // Success cases
    Json jObjInt {R"({
        "nested": 123
    })"};
    Json jInt {"123"};
    std::optional<int> got;
    ASSERT_NO_THROW(got = jObjInt.getInt("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(123, got.value());
    ASSERT_NO_THROW(got = jInt.getInt());
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(123, got.value());

    // Failure cases
    std::vector<Json> failureCases = {Json {R"({
                "nested": "value"
            })"},
                                      Json {"\"value\""},
                                      Json {R"({
                "nested": 123.456
            })"},
                                      Json {"123.456"},
                                      Json {R"({
                "nested": true
            })"},
                                      Json {"true"},
                                      Json {R"({
                "nested": false
            })"},
                                      Json {"false"},
                                      Json {R"({
                "nested": null
            })"},
                                      Json {"null"},
                                      Json {R"({
                "nested": {
                    "key": "value"
                }
            })"},
                                      Json {R"({
                "key": "value"
            })"},
                                      Json {R"({
                "nested": [
                    "value"
                ]
            })"},
                                      Json {"[\"value\"]"}};

    for (auto i = 0; i < failureCases.size(); i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_NO_THROW(got = failureCases[i].getInt("/nested"));
            ASSERT_FALSE(got.has_value());
        }
        else
        {
            ASSERT_NO_THROW(got = failureCases[i].getInt());
            ASSERT_FALSE(got.has_value());
        }
    }

    // Wrong pointer
    ASSERT_THROW(jObjInt.getInt("object/key"), std::runtime_error);
}

TEST(JsonGettersTest, GetDouble)
{
    // Success cases
    Json jObjReal {R"({
        "nested": 123.456
    })"};
    Json jReal {"123.456"};
    std::optional<double> got;
    ASSERT_NO_THROW(got = jObjReal.getDouble("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(123.456, got.value());
    ASSERT_NO_THROW(got = jReal.getDouble());
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(123.456, got.value());

    // Failure cases
    std::vector<Json> failureCases = {Json {R"({
                "nested": "value"
            })"},
                                      Json {"\"value\""},
                                      Json {R"({
                "nested": 123
            })"},
                                      Json {"123"},
                                      Json {R"({
                "nested": true
            })"},
                                      Json {"true"},
                                      Json {R"({
                "nested": false
            })"},
                                      Json {"false"},
                                      Json {R"({
                "nested": null
            })"},
                                      Json {"null"},
                                      Json {R"({
                "nested": {
                    "key": "value"
                }
            })"},
                                      Json {R"({
                "key": "value"
            })"},
                                      Json {R"({
                "nested": [
                    "value"
                ]
            })"},
                                      Json {"[\"value\"]"}};

    for (auto i = 0; i < failureCases.size(); i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_NO_THROW(got = failureCases[i].getDouble("/nested"));
            ASSERT_FALSE(got.has_value());
        }
        else
        {
            ASSERT_NO_THROW(got = failureCases[i].getDouble());
            ASSERT_FALSE(got.has_value());
        }
    }

    // Wrong pointer
    ASSERT_THROW(jObjReal.getDouble("object/key"), std::runtime_error);
}

TEST(JsonGettersTest, GetBool)
{
    // Success cases
    Json jObjBool {R"({
        "nested": true
    })"};
    Json jBool {"true"};
    std::optional<bool> got;
    ASSERT_NO_THROW(got = jObjBool.getBool("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_TRUE(got.value());
    ASSERT_NO_THROW(got = jBool.getBool());
    ASSERT_TRUE(got.has_value());
    ASSERT_TRUE(got.value());

    Json jObjBool2 {R"({
        "nested": false
    })"};
    Json jBool2 {"false"};
    ASSERT_NO_THROW(got = jObjBool2.getBool("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_FALSE(got.value());
    ASSERT_NO_THROW(got = jBool2.getBool());
    ASSERT_TRUE(got.has_value());
    ASSERT_FALSE(got.value());

    // Failure cases
    std::vector<Json> failureCases = {Json {R"({
                "nested": "value"
            })"},
                                      Json {"\"value\""},
                                      Json {R"({
                "nested": 123
            })"},
                                      Json {"123"},
                                      Json {R"({
                "nested": 123.456
            })"},
                                      Json {"123.456"},
                                      Json {R"({
                "nested": {
                    "key": "value"
                }
            })"},
                                      Json {R"({
                "key": "value"
            })"},
                                      Json {R"({
                "nested": [
                    "value"
                ]
            })"},
                                      Json {"[\"value\"]"}};

    for (auto i = 0; i < failureCases.size(); i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_NO_THROW(got = failureCases[i].getBool("/nested"));
            ASSERT_FALSE(got.has_value());
        }
        else
        {
            ASSERT_NO_THROW(got = failureCases[i].getBool());
            ASSERT_FALSE(got.has_value());
        }
    }

    // Wrong pointer
    ASSERT_THROW(jObjBool.getBool("object/key"), std::runtime_error);
}

TEST(JsonGettersTest, GetArray)
{
    // Success cases
    Json jObjArray {R"({
        "nested": [
            "value1",
            "value2"
        ]
    })"};
    Json jArray {"[\"value1\", \"value2\"]"};
    std::optional<std::vector<Json>> got;
    ASSERT_NO_THROW(got = jObjArray.getArray("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(2, got.value().size());
    ASSERT_EQ("value1", got.value()[0].getString().value());
    ASSERT_EQ("value2", got.value()[1].getString().value());
    ASSERT_NO_THROW(got = jArray.getArray());
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(2, got.value().size());
    ASSERT_EQ("value1", got.value()[0].getString().value());
    ASSERT_EQ("value2", got.value()[1].getString().value());

    // Failure cases
    std::vector<Json> failureCases = {Json {R"({
                "nested": "value"
            })"},
                                      Json {"\"value\""},
                                      Json {R"({
                "nested": 123
            })"},
                                      Json {"123"},
                                      Json {R"({
                "nested": 123.456
            })"},
                                      Json {"123.456"},
                                      Json {R"({
                "nested": true
            })"},
                                      Json {"true"},
                                      Json {R"({
                "nested": false
            })"},
                                      Json {"false"},
                                      Json {R"({
                "nested": null
            })"},
                                      Json {"null"},
                                      Json {R"({
                "nested": {
                    "key": "value"
                }
            })"},
                                      Json {R"({
                "key": "value"
            })"}};

    for (auto i = 0; i < failureCases.size(); i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_NO_THROW(got = failureCases[i].getArray("/nested"));
            ASSERT_FALSE(got.has_value());
        }
        else
        {
            ASSERT_NO_THROW(got = failureCases[i].getArray());
            ASSERT_FALSE(got.has_value());
        }
    }

    // Wrong pointer
    ASSERT_THROW(jObjArray.getArray("object/key"), std::runtime_error);
}

TEST(JsonGettersTest, GetObject)
{
    // Success cases
    Json jObjObject {R"({
        "nested": {
            "key1": "value1",
            "key2": "value2"
        }
    })"};
    Json jObject {R"({
        "key1": "value1",
        "key2": "value2"
    })"};
    std::optional<std::vector<std::tuple<std::string, Json>>> got;
    ASSERT_NO_THROW(got = jObjObject.getObject("/nested"));
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(2, got.value().size());
    ASSERT_EQ("key1", std::get<0>(got.value()[0]));
    ASSERT_EQ("value1", std::get<1>(got.value()[0]).getString().value());
    ASSERT_EQ("key2", std::get<0>(got.value()[1]));
    ASSERT_EQ("value2", std::get<1>(got.value()[1]).getString().value());
    ASSERT_NO_THROW(got = jObject.getObject());
    ASSERT_TRUE(got.has_value());
    ASSERT_EQ(2, got.value().size());
    ASSERT_EQ("key1", std::get<0>(got.value()[0]));
    ASSERT_EQ("value1", std::get<1>(got.value()[0]).getString().value());
    ASSERT_EQ("key2", std::get<0>(got.value()[1]));
    ASSERT_EQ("value2", std::get<1>(got.value()[1]).getString().value());

    // Failure cases
    std::vector<Json> failureCases = {Json {R"({
                "nested": "value"
            })"},
                                      Json {"\"value\""},
                                      Json {R"({
                "nested": 123
            })"},
                                      Json {"123"},
                                      Json {R"({
                "nested": 123.456
            })"},
                                      Json {"123.456"},
                                      Json {R"({
                "nested": true
            })"},
                                      Json {"true"},
                                      Json {R"({
                "nested": false
            })"},
                                      Json {"false"},
                                      Json {R"({
                "nested": null
            })"},
                                      Json {"null"},
                                      Json {R"({
                "nested": ["value"]
            })"},
                                      Json {R"([
                "value"
            ])"}};

    for (auto i = 0; i < failureCases.size(); i++)
    {
        if (i % 2 == 0)
        {
            ASSERT_NO_THROW(got = failureCases[i].getObject("/nested"));
            ASSERT_FALSE(got.has_value());
        }
        else
        {
            ASSERT_NO_THROW(got = failureCases[i].getObject());
            ASSERT_FALSE(got.has_value());
        }
    }

    // Wrong pointer
    ASSERT_THROW(jObjObject.getObject("object/key"), std::runtime_error);
}

/****************************************************************************************/
// SETTERS
/****************************************************************************************/
TEST(JsonSettersTest, SetString)
{
    Json jObjString {R"({
        "nested": "value"
    })"};
    Json jString {"\"value\""};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjString.setString("newValue", "/nested"));
    ASSERT_EQ("newValue", jObjString.getString("/nested").value());
    ASSERT_NO_THROW(jString.setString("newValue"));
    ASSERT_EQ("newValue", jString.getString().value());
    ASSERT_NO_THROW(jEmpty.setString("newValue"));
    ASSERT_EQ("newValue", jEmpty.getString().value());
    ASSERT_NO_THROW(jObjEmpty.setString("newValue", "/nested"));
    ASSERT_EQ("newValue", jObjEmpty.getString("/nested").value());

    // Invalid pointer
    ASSERT_THROW(jObjString.setString("newValue", "object/key"), std::runtime_error);
}

TEST(JsonSettersTest, SetInt)
{
    Json jObjInt {R"({
        "nested": 123
    })"};
    Json jInt {"123"};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjInt.setInt(456, "/nested"));
    ASSERT_EQ(456, jObjInt.getInt("/nested").value());
    ASSERT_NO_THROW(jInt.setInt(456));
    ASSERT_EQ(456, jInt.getInt().value());
    ASSERT_NO_THROW(jEmpty.setInt(456));
    ASSERT_EQ(456, jEmpty.getInt().value());
    ASSERT_NO_THROW(jObjEmpty.setInt(456, "/nested"));
    ASSERT_EQ(456, jObjEmpty.getInt("/nested").value());

    // Invalid pointer
    ASSERT_THROW(jObjInt.setInt(456, "object/key"), std::runtime_error);
}

TEST(JsonSettersTest, SetDouble)
{
    Json jObjDouble {R"({
        "nested": 123.456
    })"};
    Json jDouble {"123.456"};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjDouble.setDouble(789.012, "/nested"));
    ASSERT_EQ(789.012, jObjDouble.getDouble("/nested").value());
    ASSERT_NO_THROW(jDouble.setDouble(789.012));
    ASSERT_EQ(789.012, jDouble.getDouble().value());
    ASSERT_NO_THROW(jEmpty.setDouble(789.012));
    ASSERT_EQ(789.012, jEmpty.getDouble().value());
    ASSERT_NO_THROW(jObjEmpty.setDouble(789.012, "/nested"));
    ASSERT_EQ(789.012, jObjEmpty.getDouble("/nested").value());

    // Invalid pointer
    ASSERT_THROW(jObjDouble.setDouble(789.012, "object/key"), std::runtime_error);
}

TEST(JsonSettersTest, SetBool)
{
    Json jObjBool {R"({
        "nested": true
    })"};
    Json jBool {"true"};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjBool.setBool(false, "/nested"));
    ASSERT_EQ(false, jObjBool.getBool("/nested").value());
    ASSERT_NO_THROW(jBool.setBool(false));
    ASSERT_EQ(false, jBool.getBool().value());
    ASSERT_NO_THROW(jEmpty.setBool(false));
    ASSERT_EQ(false, jEmpty.getBool().value());
    ASSERT_NO_THROW(jObjEmpty.setBool(false, "/nested"));
    ASSERT_EQ(false, jObjEmpty.getBool("/nested").value());

    // Invalid pointer
    ASSERT_THROW(jObjBool.setBool(false, "object/key"), std::runtime_error);
}

TEST(JsonSettersTest, SetArray)
{
    Json jObjArray {R"({
        "nested": ["value"]
    })"};
    Json jArray {R"([
        "value"
    ])"};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjArray.setArray("/nested"));
    ASSERT_TRUE(jObjArray.isArray("/nested"));
    ASSERT_EQ(0, jObjArray.size("/nested"));
    ASSERT_NO_THROW(jArray.setArray());
    ASSERT_TRUE(jArray.isArray());
    ASSERT_EQ(0, jArray.size());
    ASSERT_NO_THROW(jEmpty.setArray());
    ASSERT_TRUE(jEmpty.isArray());
    ASSERT_EQ(0, jEmpty.size());
    ASSERT_NO_THROW(jObjEmpty.setArray("/nested"));
    ASSERT_TRUE(jObjEmpty.isArray("/nested"));
    ASSERT_EQ(0, jObjEmpty.size("/nested"));

    // Invalid pointer
    ASSERT_THROW(jObjArray.setArray("object/key"), std::runtime_error);
}

TEST(JsonSettersTest, SetObject)
{
    Json jObjObject {R"({
        "nested": {
            "key": "value"
        }
    })"};
    Json jObject {R"({
        "key": "value"
    })"};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjObject.setObject("/nested"));
    ASSERT_TRUE(jObjObject.isObject("/nested"));
    ASSERT_EQ(0, jObjObject.size("/nested"));
    ASSERT_NO_THROW(jObject.setObject());
    ASSERT_TRUE(jObject.isObject());
    ASSERT_EQ(0, jObject.size());
    ASSERT_NO_THROW(jEmpty.setObject());
    ASSERT_TRUE(jEmpty.isObject());
    ASSERT_EQ(0, jEmpty.size());
    ASSERT_NO_THROW(jObjEmpty.setObject("/nested"));
    ASSERT_TRUE(jObjEmpty.isObject("/nested"));
    ASSERT_EQ(0, jObjEmpty.size("/nested"));

    // Invalid pointer
    ASSERT_THROW(jObjObject.setObject("object/key"), std::runtime_error);
}

TEST(JsonSettersTest, AppendString)
{
    Json jObjString {R"({
        "nested": ["value"]
    })"};
    Json jObjStringOverwrite {R"({
        "nested": 1
    })"};
    Json jString {"[\"value\"]"};
    Json jStringOverwrite {"1"};
    Json jEmpty {};
    Json jObjEmpty {};
    ASSERT_NO_THROW(jObjString.appendString("value2", "/nested"));
    ASSERT_EQ(jObjString.size("/nested"), 2);
    ASSERT_EQ(jObjString.getString("/nested/0"), "value");
    ASSERT_EQ(jObjString.getString("/nested/1"), "value2");
    ASSERT_NO_THROW(jObjStringOverwrite.appendString("value2", "/nested"));
    ASSERT_EQ(jObjStringOverwrite.size("/nested"), 1);
    ASSERT_EQ(jObjStringOverwrite.getString("/nested/0"), "value2");
    ASSERT_NO_THROW(jString.appendString("value2"));
    ASSERT_EQ(jString.size(), 2);
    ASSERT_EQ(jString.getString("/0"), "value");
    ASSERT_EQ(jString.getString("/1"), "value2");
    ASSERT_NO_THROW(jStringOverwrite.appendString("value2"));
    ASSERT_EQ(jStringOverwrite.size(), 1);
    ASSERT_EQ(jStringOverwrite.getString("/0"), "value2");
    ASSERT_NO_THROW(jEmpty.appendString("value2"));
    ASSERT_EQ(jEmpty.size(), 1);
    ASSERT_EQ(jEmpty.getString("/0"), "value2");
    ASSERT_NO_THROW(jObjEmpty.appendString("value2", "/nested"));
    ASSERT_EQ(jObjEmpty.size("/nested"), 1);
    ASSERT_EQ(jObjEmpty.getString("/nested/0"), "value2");

    // Invalid pointer
    ASSERT_THROW(jObjString.appendString("object/key", "value2"), std::runtime_error);
}

TEST(JsonSettersTest, Erase)
{
    Json jObj {R"({
        "nested": {
            "key": "value"
        }
    })"};
    ASSERT_TRUE(jObj.erase("/nested/key"));
    ASSERT_EQ(jObj.size("/nested"), 0);
    ASSERT_FALSE(jObj.erase("/nested/key"));
    ASSERT_TRUE(jObj.erase());
    ASSERT_TRUE(jObj.isNull());

    // Invalid pointer
    ASSERT_THROW(jObj.erase("object/key"), std::runtime_error);
}
