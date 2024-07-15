#ifndef TOPCHECK_HPP
#define TOPCHECK_HPP

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Messaging {
	enum class MessageType {
		UNKNOWN = -1,
		FAIL = 0,
		SUCCESS = 1,
		WARNING = 2,
		INFO = 3
	};

	struct Message {
	public:
		std::string content_;
		MessageType type_;
		std::time_t timestamp_;

		Message() : content_(""), type_(MessageType::UNKNOWN), timestamp_(std::time(nullptr)) 
		{}
		
		Message(const std::string& content, MessageType type)
			: content_(content), type_(type), timestamp_(std::time(nullptr)) 
		{}

		Message(const Message& other)
			: content_(other.content_), type_(other.type_), timestamp_(other.timestamp_)
		{}

		Message(Message&& other) noexcept
			: content_(std::move(other.content_)), type_(other.type_), timestamp_(other.timestamp_)
		{
			other.type_ = MessageType::UNKNOWN;
			other.timestamp_ = 0;
		}

		Message& operator=(const Message& other) {
			if (this != &other) {
				content_ = other.content_;
				type_ = other.type_;
				timestamp_ = other.timestamp_;
			}
			return *this;
		}

		Message& operator=(Message&& other) noexcept {
			if (this != &other) {
				content_ = std::move(other.content_);
				type_ = other.type_;
				timestamp_ = other.timestamp_;

				other.type_ = MessageType::UNKNOWN;
				other.timestamp_ = 0;
			}
			return *this;
		}

		std::string getFormattedTimestamp() const {
			char buffer[80];
			struct tm* timeinfo = localtime(&timestamp_);
			strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
			return std::string(buffer);
		}

		std::string format() const {
			std::stringstream ss;
			ss << "[" << getFormattedTimestamp() << "] ";
			ss << "[" << messageTypeToString(type_) << "] ";
			ss << content_;
			return ss.str();
		}

		static std::string messageTypeToString(MessageType type) {
			switch (type) 
			{
				case MessageType::FAIL: return "FAIL";
				case MessageType::SUCCESS: return "SUCCESS";
				case MessageType::WARNING: return "WARNING";
				case MessageType::INFO:	 return "INFO";
				default: return "UNKNOWN";
			}
		}

		std::string serialize() const {
			std::stringstream ss;
			ss << content_ << "|" << static_cast<int>(type_) << "|" << timestamp_;
			return ss.str();
		}

		static Message deserialize(const std::string& data) {
			std::stringstream ss(data);
			std::string content;
			int type;
			std::time_t timestamp;
			std::getline(ss, content, '|');
			ss >> type;
			ss.ignore(1);
			ss >> timestamp;

			return Message(content, static_cast<MessageType>(type), timestamp);
		}
	private:
		// Deserialization constructor
		Message(const std::string& content, MessageType type, std::time_t timestamp)
			: content_(content), type_(type), timestamp_(timestamp)
		{}
	};

}  // namespace Messaging

template<typename T>
concept EqualityComparable = requires(T a, T b) {
	{ a == b } -> std::convertible_to<bool>;
};

template<typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

namespace Topcheck
{	
	// Base class for test cases to enable polymorphism
	class ITestCase 
	{
	public:
		virtual ~ITestCase() = default;
		virtual void Run() const = 0;
		virtual const std::string& GetName() const = 0;
	};

	template<typename Func>
	class TestCase : public ITestCase
	{
	public:
		TestCase(const std::string& name, Func testFunc)
			: m_Name(name), m_TestFunc(testFunc)
		{}

		void Run() const override
		{
			if constexpr (std::is_void_v<std::invoke_result_t<Func>>)
			{
				m_TestFunc();
			}
			else 
			{
				auto result = m_TestFunc();
				std::cout << "Test returned: " << result << std::endl;
			}
		}

		const std::string& GetName() const override { return m_Name; }
	private:
		std::string m_Name;
		Func m_TestFunc;

	};  // class TestCase 
	
	class TestRunner
	{
	public:
		template<typename Func>
		void AddTestCase(const std::string& name, Func testFunc)
		{
			m_TestCases.push_back(std::make_unique<TestCase<Func>>(name, testFunc));
		}

		void RunAll() const
		{
			for (const auto& testCase : m_TestCases)
			{
				std::cout << "Running test: " << testCase->GetName() << std::endl;
				testCase->Run();
			}
		}

	private:
		std::vector<std::unique_ptr<ITestCase>> m_TestCases;
	};  // class TestRunner
	
	
	class Assertion 
	{
	public:
		// Constructors
		Assertion() = default;

		Assertion(const std::string& file)
			: m_File(file) {}
		
		Assertion(const std::string& file, int line)
			: m_File(file), m_Line(line) {}

		template<EqualityComparable T>
		void AssertEqual(const T& expected, const T& actual)
		{	
			std::ostringstream oss;
			oss << expected << "==" << actual;

			if (!(expected == actual))
				buildFailMessage(oss.str());
			else
				buildSuccessMessage(oss.str());
		}

		template<FloatingPoint T>
		void AssertAlmostEqual(const T& expected, const T& actual, const T& epsilon) 
		{
			std::ostringstream oss;
			oss << "Expected: " << expected << ", Actual: " << actual << ", Epsilon: " << epsilon;

			if (std::fabs(expected - actual) >= epsilon)
				buildFailMessage(oss.str());
			else
				buildSuccessMessage(oss.str());
		}

		void PrintResults() const 
		{
			for (const auto& message : m_Messages)
			{
				std::cout << message << std::endl;
			}
		}

		bool HasFailures() const
		{
			return !m_FailMessages.empty();
		}

	private:
		void buildFailMessage(const std::string& message) 
		{
			std::ostringstream oss;
			oss << "Test Failed: ";
			
			if (!message.empty())
				oss << message;
			
			if (!m_File.empty() && m_Line != 0)
				oss << " at " << m_File << ":" << m_Line;

			m_Messages.push_back(oss.str());
			m_FailMessages.push_back(oss.str());
		}

		void buildSuccessMessage(const std::string& message) 
		{
			std::ostringstream oss;
			oss << "Test Passed: ";
			if (!message.empty()) 
				oss << message;

			m_Messages.push_back(oss.str());
		}

		std::string m_File;
		int m_Line = 0;
		std::vector<std::string> m_Messages;
		std::vector<std::string> m_FailMessages;
	};  // class Assertion

}  // namespace Topcheck
#endif  // TOPCHECK_HPP