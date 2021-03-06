#pragma once
/////////////////////////////////////////////////////////////////////
// HttpMessage.h - Build and parse Http style messages             //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
/////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <array>

//----< Class definition >--------

class HttpMessage
{
public:
	using byte = char;
	using Name = std::string;
	using Value = std::string;
	using Attribute = std::pair<Name, Value>;
	using Attributes = std::vector<Attribute>;
	using Terminator = std::string;
	using Body = std::vector<byte>;

//----< message attributes >--------
	void addAttribute(const Attribute& attrib);
	Value findValue(const Name& name);
	size_t findAttribute(const Name& name);
	bool removeAttribute(const Name& name);
	Attributes& attributes();
	static std::string attribString(const Attribute& attrib);
	static Attribute attribute(const Name& name, const Value& value);
	static Attribute parseAttribute(const std::string& src);
	static HttpMessage parseMessage(std::string& src);
	std::vector<std::string> getTypeVectr();


//----< message body manipulation >--------
	void setBody(byte buffer[], size_t Buflen);
	size_t getBody(byte buffer[], size_t& Buflen);
	void addBody(const Body& body);
	void addBody(const std::string& body);
	void addBody(size_t numBytes, byte* pBuffer);
	Body& body();
	size_t bodyLength();

//----< display methods >--------
	std::string headerString() const;
	std::string bodyString() const;
	std::string toString() const;
	std::string toIndentedHeaderString() const;
	std::string toIndentedBodyString() const;
	std::string toIndentedString() const;
	std::string patternsString() const;

//----< clean up >--------
	void clear();
	static void fillBuffer(byte buffer[], size_t BufSize, byte fill = '\0');
	void addPattern(std::string pattern);
	void parsePatternString(std::string);

//----< private statements >--------
private:
	Attributes attributes_;
	Terminator term_ = "\n";
	Body body_;
	std::vector<std::string> typeVector;
};