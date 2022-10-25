#pragma once

class Error;

/**
 * Represents all the types a Json Value can be. 
 */
enum class EJson
{
    None,
    Null,
    String,
    Number,
    Boolean,
    Array,
    Object
};

enum class EJsonToken
{
    None,
    Comma,
    CurlyOpen,
    CurlyClose,
    SquareOpen,
    SquareClose,
    Colon,
    String,

    // short values
    Number,
    True,
    False,
    Null,

    Identifier
};

inline bool EJsonToken_IsShortValue(EJsonToken Token)
{
    return Token >= EJsonToken::Number && Token <= EJsonToken::Null;
}

enum class EJsonNotation
{
    ObjectStart,
    ObjectEnd,
    ArrayStart,
    ArrayEnd,
    Boolean,
    String,
    Number,
    Null,
    Error
};
