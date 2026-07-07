#pragma once

#define BIT(x) (1u << x)

namespace xc
{
	template<typename TEnum>
	class EnumFlags
	{
	public:
		using TType = std::underlying_type_t<TEnum>;

		EnumFlags()
			: m_Flags(ToEnum(0))
		{
		}

		EnumFlags(const TEnum flags)
			: m_Flags(flags)
		{
		}

		explicit EnumFlags(const TType flags)
			: m_Flags(ToEnum(flags))
		{
		}

		EnumFlags<TEnum> operator |(const EnumFlags<TEnum> flags) const
		{
			return EnumFlags(GetValue() | flags.GetValue());
		}

		EnumFlags<TEnum> operator |=(const EnumFlags<TEnum> flags)
		{
			m_Flags = ToEnum(GetValue() | flags.GetValue());
			return *this;
		}

		bool operator==(const EnumFlags<TEnum> flags) const
		{
			return GetValue() == flags.GetValue();
		}

		void Raise(const TEnum flag)
		{
			m_Flags = ToEnum(GetValue() | ToValue(flag));
		}

		void Clear(const TEnum flag)
		{
			m_Flags = ToEnum(ToValue(m_Flags) & ~ToValue(flag));
		}

		void ClearAll()
		{
			m_Flags = ToEnum(0);
		}

		bool Has(const TEnum flag) const
		{
			return GetValue() & ToValue(flag);
		}

		bool HasAll(const EnumFlags<TEnum> flags) const
		{
			return (GetValue() & flags.GetValue()) == flags.GetValue();
		}

		bool HasAny() const
		{
			return GetValue() != 0;
		}

		bool HasAny(const EnumFlags<TEnum> flags) const
		{
			return (GetValue() & flags.GetValue()) != 0;
		}

		TEnum Get() const
		{
			return m_Flags;
		}

		TType GetValue() const
		{
			return ToValue(m_Flags);
		}

	private:

		static TType ToValue(TEnum flag)
		{
			return static_cast<TType>(flag);
		}

		static TEnum ToEnum(TType value)
		{
			return static_cast<TEnum>(value);
		}

		TEnum m_Flags;
	};
}