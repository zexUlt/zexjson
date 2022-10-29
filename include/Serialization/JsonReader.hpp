#pragma once

#include "Minimal.hpp"
#include "Serialization/JsonTypes.hpp"


namespace zexjson{

class Error;

#define JSON_NOTATIONMAP_DEF \
static EJsonNotation TokenToNotationTable[] = \
{ \
    EJsonNotation::Error,       /*EJsonToken::None*/ \
    EJsonNotation::Error,       /*EJsonToken::Comma*/ \
    EJsonNotation::ObjectStart, /*EJsonToken::CurlyOpen*/ \
    EJsonNotation::ObjectEnd,   /*EJsonToken::CurlyClose*/ \
    EJsonNotation::ArrayStart,  /*EJsonToken::SquareOpen*/ \
    EJsonNotation::ArrayEnd,    /*EJsonToken::SquareClose*/ \
    EJsonNotation::Error,       /*EJsonToken::Colon*/ \
    EJsonNotation::String,      /*EJsonToken::String*/ \
    EJsonNotation::Number,      /*EJsonToken::Number*/ \
    EJsonNotation::Boolean,     /*EJsonToken::True*/ \
    EJsonNotation::Boolean,     /*EJsonToken::False*/ \
    EJsonNotation::Null,        /*EJsonToken::Null*/ \
};

#ifndef WITH_JSON_INLINED_NOTATIONMAP
#define WITH_JSON_INLINED_NOTATIONMAP 0
#endif // WITH_JSON_INLINED_NOTATIONMAP

#if !WITH_JSON_INLINED_NOTATIONMAP
JSON_NOTATIONMAP_DEF;
#endif // WITH_JSON_INLINED_NOTATIONMAP

template<class CharType>
class JsonReader
{
public:
    static std::shared_ptr<JsonReader<CharType>> Create(std::istream* const Stream)
    {
        return std::make_shared(new JsonReader<CharType>(Stream));
    }

public:
    virtual ~JsonReader() = default;

    bool ReadNext(EJsonNotation& Notation)
    {
        if(!ErrorMessage.empty()){
            Notation = EJsonNotation::Error;
            return false;
        }

        if(!Stream){
            Notation = EJsonNotation::Error;
            SetErrorMessage(L"Null Stream");
            return true;
        }

        const bool AtEndOfStream = Stream->eof();

        if(AtEndOfStream && !FinishedReadingRootObject){
            Notation = EJsonNotation::Error;
            SetErrorMessage(L"Improperly formatted.");
            return true;
        }

        if(FinishedReadingRootObject && !AtEndOfStream){
            Notation = EJsonNotation::Error;
            SetErrorMessage(L"Unexpected additional input found.");
            return true;
        }

        if(AtEndOfStream){
            return false;
        }

        bool ReadWasSuccess = true;
        Identifier.clear();

        do{
            EJson CurrentState = EJson::None;

            if(ParseState.size() > 0){
                CurrentState = ParseState.back();
            }

            switch (expression)
            {
            case EJson::Array:
                ReadWasSuccess = ReadNextArrayValue( /* OUT */ CurrentToken);
                break;

            case EJson::Object:
                ReadWasSuccess = ReadNextObjectvalue(/* OUT */ CurrentToken);
                break;

            default:
                ReadWasSuccess = ReadStart( /* OUT */ CurrentToken);
                break;
            }
        }while(ReadWasSuccess && (CurrentToken == EJsonToken::None));

#if WITH_JSON_INLINED_NOTATIONMAP
        JSON_NOTATIONMAP_DEF;
#endif // WITH_JSON_INLINED_NOTATIONMAP

        Notation = TokenToNotationTable[(std::int32_t)CurrentToken];
        FinishedReadingRootObject = ParseState.size() == 0;

        if(!ReadWasSuccess || Notation == EJsonNotation::Error){
            Notation = EJsonNotation::Error;

            if(ErrorMessage.empty()){
                SetErrorMessage(L"Unknown Error Occured");
            }

            return true;
        }

        if(FinishedReadingRootObject && !Stream->eof()){
            ReadWasSuccess = ParseWhiteSpace();
        }

        return ReadWasSuccess;
    }

    bool SkipObject()
    {
        return ReadUntilMatching(EJsonNotation::ObjectEnd);
    }

    bool SkipArray()
    {
        return ReadUntilMatching(EJsonNotation::ArrayEnd);
    }

    inline virtual const std::string& GetIdentifier() const { return Identifier; }

    inline virtual const std::string& GetValueAsString() const
    {
        assert(CurrentToken == EJsonToken::String);
        return StringValue;
    }

    inline double GetValueAsNumber() const
    {
        assert(CurrentToken == EJsonToken::Number);
        return NumberValue;
    } 

    inline const std::string& GetValueAsNumberString() const
    {
        assert(CurrentToken == EJsonToken::Number);
        return StringValue;
    }

    inline bool GetValueAsBoolean() const
    {
        assert((CurrentToken == EJsonToken::True || CurrentToken == EJsonToken::False));
        return BoolValue;
    }

    inline const std::string& GetErrorMessage() const
    {
        return ErrorMessage;
    }

    inline const std::uint32_t GetLineNumber() const
    {
        return LineNumber;
    }

    inline const std::uint32_t GetCharacterNumber() const
    {
        return CharacterNumber;
    }

protected:

    /* Hidden default constructor. */
    JsonReader() :
        ParserState(), CurrentToken(EJsonToken::None), Stream(nullptr),
        Identifier(), ErrorMessage(), StringValue(), NumberValue(0.f),
        LineNumber(1), CharacterNumber(0), BoolValue(false), FinishedReadingRootObject(false)
        {}

    /**
     * Creates and initializes a new instance with the given input.
     * 
     * @param InStream A stream providing the input.
    */
    JsonReader(std::istream* InStream) :
        ParserState(), CurrentToken(EJsonToken::None), Stream(InStream),
        Identifier(), ErrorMessage(), StringValue(), NumberValue(0.f),
        LineNumber(1), CharacterNumber(0), BoolValue(false), FinishedReadingRootObject(false)
        {}

    bool Serialize(void* V, std::int64_t Length)
    {
        Stream->read(V, Length);
        if(Stream->fail()){
            SetErrorMessage(L"Stream I/O Error.");
            return false;
        } 
        return true;
    }

    std::vector<EJson> ParseState;
    EJsonToken CurrentToken;

    std::unique_ptr<std::istream> Stream;
    std::string Identifier;
    std::string ErrorMessage;
    std::string StringValue;
    double NumberValue;
    std::uint32_t LineNumber;
    std::uint32_t CharacterNumber;
    bool BoolValue;
    bool FinishedReadingRootObject;

private:
    void SetErrorMessage(const std::string& Message)
    {
        ErrorMessage = Message + 
            " Line: " + std::to_string(LineNumber) + 
            " Ch: " + std::to_string(CharacterNumber);
    }

    bool ReadUntilMatching(const EJsonNotation ExpectedNotation)
    {
        std::uint32_t ScopeCount = 0;
        EJsonNotation Notation;

        while(ReadNext(Notation)){
            if(ScopeCount == 0 && Notation == ExpectedNotation){
                return true;
            }

            switch (Notation)
            {
            case EJsonNotation::ObjectStart:
            case EJsonNotation::ArrayStart:
                ScopeCount++;
                break;

            case EJsonNotation::ObjectEnd:
            case EJsonNotation::ArrayEnd:
                ScopeCount--;
                break;
            
            case EJsonNotation::Boolean:
            case EJsonNotation::Null:
            case EJsonNotation::Number:
            case EJsonNotation::String:
                break;
            
            case EJsonNotation::Error:
                return false;
            }
        }

        return !Stream->bad();
    }

    bool ReadStart(EJsonToken& Token)
    {
        if(!ParseWhiteSpace()){
            return false;
        }

        Token = EJsonToken::None;

        if(NextToken(Token) == false){
            return false;
        }

        if(Token != EJsonToken::CurlyOpen && Token != EJsonToken::SquareOpen){
            SetErrorMessage(L"Open Curly or Square Brace token expected, but not found.");
            return false;
        }

        return true;
    }

    bool ReadNextObjectValue(EJsonToken& Token)
    {
        const bool bCommaPrepend = Token != EJsonToken::CurlyOpen;
        Token = EJsonToken::None;

        if(!NextToken(Token)){
            return false;
        } 

        if(Token == EJsonToken::CurlyClose){
            return true;
        }else{
            if(bCommaPrepend){
                if(Token != EJsonToken::Comma){
                    SetErrorMessage(L"Comma token expected, but not found.");
                    return false;
                }

                Token = EJsonToken::None;

                if(!NextToken(Token)){
                    return false;
                }
            }

            if(Token != EJsonToken::String){
                SetErrorMessage(L"String token expected, but not found.")
            }
            
            Identifier = StringValue;
            Token = EJsonToken::None;

            if(!NextToken(Token)){
                return false;
            }

            if(Token != EJsonToken::Colon){
                SetErrorMessage(L"Colon token expected, but not found.");
                return false;
            }

            Token = EJsonToken::None;

            if(!NextToken(Token)){
                return false;
            }
        }

        return true;
    }

    bool ReadNextArrayValue(EJsonToken& Token)
    {
        const bool bCommaPrepend = Token != EJsonToken::SquareOpen;

        Token = EJsonToken::None;

        if(!NextToken(Token)){
            return false;
        }

        if(Token == EJsonToken::SquareClose){
            return true;
        }else{
            if(bCommaPrepend){
                if(Token != EJsonToken::Comma){
                    SetErrorMessage(L"Comma token expected, but not found.");
                    return false;
                }

                Token = EJsonToken::None;

                if(!NextToken(Token)){
                    return false;
                }
            }
        }

        return true;
    }

    bool NextToken(EJsonToken& OutToken)
    {
        while(!Stream->eof()){
            CharType Char;

            if(!Serialize(&Char, sizeof(CharType))){
                return false;
            }
            ++CharacterNumber;

            if(Char == CharType('\0')){
                break;
            }

            if(IsLineBreak(Char)){
                ++LineNumber;
                CharacterNumber = 0;
            }

            if(!IsWhitespace(Char)){
                if(IsJsonNumber(Char)){
                    if(!ParseNumberToken(Char)){
                        return false;
                    }

                    OutToken = EJsonToken::Number;
                    return true;
                }

                switch (Char)
                {
                case CharType('{'):
                    OutToken = EJsonToken::CurlyOpen;
                    ParseState.push_back(EJson::Object);
                    return true;
                
                case CharType('}'):
                {
                    OutToken = EJsonToken::CurlyClose;
                    if(ParseState.size()){
                        ParseState.pop_back();
                        return true;
                    }else{
                        SetErrorMessage(L"Unknown state reached while parsing Json token.");
                        return false;
                    }
                }
                }

                case CharType('['):
                    OutToken = EJsonToken::SquareOpen;
                    ParseState.push_back(EJson::Array);
                    return true;

                case CharType(']'):
                {
                    OutToken = EJsonToken::SquareClose;
                    if(ParseState.size()){
                        ParseState.pop_back();
                        return true;
                    }else{
                        SetErrorMessage(L"Unknown state reached while parsing Json token.");
                        return false;
                    }
                }

                case CharType(':'):
                    OutToken = EJsonToken::Colon;
                    return true;
                
                case CharType(','):
                    OutToken = EJsonToken::Comma;
                    return true;
                
                case CharType('\"'):
                {
                    if(!ParseStringToken()){
                        return false;
                    }

                    OutToken = EJsonToken::String;
                    return true;
                }

                case CharType('t'): case CharType('T'):
                case CharType('f'): case CharType('F'):
                {
                    std::string Test;
                    Test += Char;

                    while(!Stream->eof()){
                        if(!Serialize(&Char, sizeof(CharType))){
                            return false;
                        }

                        if(IsAlphaNumber(Char)){
                            ++CharacterNumber;
                            Test += Char;
                        }else{
                            // backtrack and break
                            Stream->seekg(Stream->tellg() - sizeof(CharType));
                            break;
                        }
                    }

                    if(Test == L"False"){
                        BoolValue = false;
                        OutToken = EJsonToken::False;
                        return true;
                    }

                    if(Test == L"True"){
                        BoolValue = true;
                        OutToken = EJsonToken::True;
                        return true;
                    }

                    if(Test == L"Null"){
                        OutToken = EJsonToken::Null;
                        return true;
                    }

                    SetErrorMessage(L"Invalid Json Token. Check that your member names have quotes around them!");
                    return false;
                }

                default:
                    SetErrorMessage(L"Invalid Json Token.");
                    return false;
            }
        }

        SetErrorMessage(L"Invalid Json Token.");
        return false;
    }

    bool ParseStringToken()
    {
        std::string String;

        while(true){
            if(Stream->eof()){
                SetErrorMessage(L"String Token Abruptly Ended.");
                return false;
            }

            CharType Char;
            if(!Serialize(&Char, sizeof(CharType))){
                return false;
            }
            ++CharacterNumber;

            if(Char == CharType('\"')){
                break;
            }

            if(Char == CharType('\\')){
                if(!Serialize(&Char, sizeof(CharType))){
                    return false;
                }
                ++CharacterNumber;

                switch (Char)
                {
                case CharType('\"'): case CharType('\\'): case CharType('/'): String += Char; break;
                case CharType('f'): String += CharType('\f'); break;
                case CharType('r'): String += CharType('\r'); break;
                case CharType('n'): String += CharType('\n'); break;
                case CharType('b'): String += CharType('\b'); break;
                case CharType('t'): String += CharType('\t'); break;
                case CharType('u'):
                // 4 hex digits, like \uFF00, which is 16 bit number that we would usually see as 0xFF00
                {
                    std::int32_t HexNum = 0;

                    for(std::int32_t Radix = 3; Radix >= 0; --Radix){
                        if(Stream->eof()){
                            SetErrorMessage(L"String token abruptly ended.");
                            return false;
                        }

                        if(!Serialize(&Char, sizeof(CharType))){
                            return false;
                        }
                        ++CharacterNumber;

                        const std::int32_t HexDigit = JsonUtils::ParseHexDigit(Char); // <- Add to JsonUtils
                        
                        if(HexDigit == 0 && Char != CharType('0')){
                            SetErrorMessage(L"Invalid hexadecimal digit parsed.");
                            return false;
                        }

                        HexNum += HexDigit * static_cast<std::int32_t>(std::pow(16, Radix));
                    }

                    String += HexNum;
                    break;
                }
                default:
                    SetErrorMessage(L"Bad Json escaped char.");
                    break;
                }
            }else{
                String += Char;
            }
        }

        StringValue = std::move(String);
        
        return true;
    }

    bool ParseNumberToken(CharType FirstChar)
    {
        std::string String;
        std::int32_t State = 0;
        bool UseFirstChar = true;
        bool StateError = false;

        while(true){
            if(Stream->eof()){
                SetErrorMessage(L"Number token abruptly ended.");
                return false;
            }

            CharType Char;
            if(UseFirstChar){
                Char = FirstChar;
                UseFirstChar = false;
            }else{
                if(!Serialize(&Char, sizeof(CharType))){
                    return false;
                }
                ++CharacterNumber;
            }

            // The following code doesn't actually derive the Json Number:
            // that is handled by the function std::stod below.
            // This code only ensures the Json Number is EXACTLY to specification
            if(IsJsonNumber(Char)){
                // Ensure number follows Json format before converting
                // This switch statement is derived from a finite state automata
                // derived from the Json spec. A table was not used for simplicity.
                switch (State){
                    case 0:
                        if(Char == CharType('-')) { State = 1; }
                        else if(Char == CharType('0')) { State = 2; }
                        else if(IsNonZeroDigit(Char))) { State = 3; }
                        else { StateError = true; }
                        break;
                    
                    case 1:
                        if(Char == CharType('0')) { State = 2; }
                        else if(IsNonZeroDigit(Char)) { State = 3; }
                        else { StateError = true; }
                        break;
                    
                    case 2:
                        if(Char == CharType('.')) { State = 4; }
                        else if(Char == CharType('e') || Char == CharType('E')) { State = 5; }
                        else { StateError = true; }
                        break;
                    
                    case 3:
                        if(IsDigit(Char)) { State = 3; }
                        else if(Char == CharType('.')) { State = 4; }
                        else if(Char == CharType('e') || Char == CharType('E')) { State = 5; }
                        else { StateError = true; }
                        break;
                    
                    case 4:
                        if(IsDigit(Char)) { State = 6; }
                        else { StateError = true; }
                        break;
                    
                    case 5:
                        if(Char == CharType('-') || Char == CharType('+')) { State = 7; }
                        else if(IsDigit(Char)) { State = 8; }
                        else { StateError = true; }
                        break;
                    
                    case 6:
                        if(IsDigit(Char)) { State = 6; }
                        else if(Char == CharType('e') || Char == CharType('E')) { State = 5; }
                        else{ StateError = true; }
                        break;

                    case 7: 
                        if(IsDigit(Char)) { State = 8; }
                        else{ StateError = true; }
                        break;
                    
                    case 8:
                        if(IsDigit(Char)) { State = 8; }
                        else{ StateError = true; }
                        break;
                    
                    default:
                        SetErrorMessage(L"Unknown state reached in Json Number token.");
                        return false;
                }

                if(StateError){
                    break;
                }

                String += Char;
            }else{
                // backtrack once because we read a non-number character
                Stream->seekg(Stream->tellg() - sizeof(CharType));
                --CharacterNumber;
                // And now the number is fully tokenized
                break;
            }
        }

        // Ensure the number has followed valid Json format
        if(!StateError && (State == 2 || State == 3 || State == 6 || State == 8)){
            StringValue = String;
            NumberValue = std::stod(String);
            return true;
        }

        SetErrorMessage(L"Poorly formed Json Number token.");
        return false;
    }

    bool ParseWhiteSpace()
    {
        while(!Stream->eof()){
            CharType Char;
            if(!Serialize(&Char, sizeof(CharType))){
                return false;
            }
            ++CharacterNumber;

            if(IsLineBreak(Char)){
                ++LineNumber;
                CharacterNumber = 0;
            }

            if(!IsWhitespace(Char)){
                // backtrack and break
                Stream->seekg(Stream->tellg() - sizeof(CharType));
                --CharacterNumber;
                break;
            }
        }

        return true;
    }

    static bool IsLineBreak(const CharType& Char)
    {
        return Char == CharType('\n');
    }

    static bool IsWhitespace(const CharType& Char)
    {
        return Char == CharType(' ') || Char == CharType('\t') || 
            Char == CharType('\n') || Char == CharType('\r');
    }

    static bool IsJsonNumber(const CharType& Char)
    {
        return (Char >= CharType('0') && Char <= CharType('9')) ||
            Char == CharType('-') || Char == CharType('.') || Char == CharType('+') || 
            Char == CharType('e') || Char == CharType('E');
    }

    static bool IsDigit(const CharType& Char)
    {
        return Char >= CharType('0') && Char <= CharType('9');
    }

    static bool IsNonZeroDigit(const CharType& Char)
    {
        return Char >= CharType('1') && Char <= CharType('9');
    }

    static bool IsAlphaNumber(const CharType& Char)
    {
        return (Char >= CharType('a') && Char <= CharType('z')) || 
               (Char >= CharType('A') && Char <= CharType('Z'));
    }

};


class JsonStringReader : public JsonReader<char>
{
public:
    static std::shared_ptr<JsonStringReader> Create(const std::string& JsonString)
    {
        return std::make_shared<JsonStringReader>(JsonString);
    }

    static std::shared_ptr<JsonStringReader> Create(std::string&& JsonString)
    {
        return std::make_shared<JsonStringReader>(std::move(JsonString));
    }

    const std::string& GetSourceString() const
    {
        return Content;
    }

    virtual ~JsonStringReader() = default;

protected:

    /**
     * Parses a string containing Json information.
     * 
     * @param JsonString The Json string to parse.
    */
    JsonStringReader(const std::string& JsonString) :
        Content(JsonString), Reader(nullptr)
    {
        InitReader();
    }

    /**
     * Parses a string containing Json information.
     * 
     * @param JsonString The Json string to parse.
    */
    JsonStringReader(std::string&& JsonString) :
        Content(std::move(JsonString)), Reader(nullptr)
    {
        InitReader();
    }

    inline void InitReader()
    {
        if(Content.empty()){
            return;
        }

        Reader = std::make_unique<std::istringstream>(Content);
        
        Stream = std::make_unique<std::istream>(Reader.get());
    }

protected:
    const std::string Content;
    std::unique_ptr<std::istringstream> Reader;
};


template<class CharType = char>
class JsonReaderFactory
{
public:
    static std::shared_ptr<JsonReader<char>> Create(const std::string& JsonString)
    {
        return JsonStringReader::Create(JsonString);
    }

    static std::shared_ptr<JsonReater<char>> Create(std::string&& JsonString)
    {
        return JsonStringReader::Create(std::move(JsonString));
    }

    static std::shared_ptr<JsonReader<CharType>> Create(std::istream* const Stream)
    {
        return JsonReader<CharType>::Create(Stream);
    }
};

} // namespace zexjson