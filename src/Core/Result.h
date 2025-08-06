#pragma once

#include <string>
#include <variant>

namespace bread::core
{
	struct ResultError
	{
		std::string m_Error;
	};

	template<typename TResult>
	class Result
	{
	public:
		Result(TResult result)
			: m_Value(std::move(result))
		{
		}

		Result(core::ResultError error)
			: m_Value(std::move(error))
		{}

		operator bool() const { return std::holds_alternative<TResult>(m_Value); }

		const TResult& value() const { return std::get<TResult>(m_Value); }
		const std::string& error() const { return std::get<core::ResultError>(m_Value).m_Error; }

	private:
		std::variant<TResult, core::ResultError> m_Value;
	};

	template<>
	class Result<void>
	{
	public:
		Result()
			: m_Success(true)
		{
		}

		Result(core::ResultError error)
			: m_Error(std::move(error))
		{
		}

		operator bool() const { return m_Success; }

		const std::string& error() const { return m_Error.m_Error; }

	private:
		bool m_Success = false;
		core::ResultError m_Error;
	};
}