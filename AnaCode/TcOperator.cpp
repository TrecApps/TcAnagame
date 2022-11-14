#include "TcOperator.h"
#include "TPrimitiveVariable.h"

DoubleLong::DoubleLong(ULONG64 val)
{
	value.u = val;
	type = double_long::dl_unsign;
}

DoubleLong::DoubleLong(LONG64 val)
{
	value.s = val;
	type = double_long::dl_sign;
}

DoubleLong::DoubleLong(double val)
{
	value.d = val;
	type = double_long::dl_double;
}

DoubleLong::DoubleLong()
{
	type = double_long::dl_invalid;
	value.u = 0ULL;
}

bool DoubleLong::operator<(const DoubleLong& o)
{
	switch (type)
	{
	case double_long::dl_double:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.d < o.value.d;
		case double_long::dl_sign:
			return value.d < o.value.s;
		case double_long::dl_unsign:
			return value.d < o.value.u;
		}
		break;
	case double_long::dl_sign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.s < o.value.d;
		case double_long::dl_sign:
			return value.s < o.value.s;
		case double_long::dl_unsign:
			return value.s < o.value.u;
		}
		break;
	case double_long::dl_unsign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.u < o.value.d;
		case double_long::dl_sign:
			return value.u < o.value.s;
		case double_long::dl_unsign:
			return value.u < o.value.u;
		}
	}

	return false;
}

bool DoubleLong::operator==(const DoubleLong& o)
{
	switch (type)
	{
	case double_long::dl_double:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.d == o.value.d;
		case double_long::dl_sign:
			return value.d == o.value.s;
		case double_long::dl_unsign:
			return value.d == o.value.u;
		}
		break;
	case double_long::dl_sign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.s == o.value.d;
		case double_long::dl_sign:
			return value.s == o.value.s;
		case double_long::dl_unsign:
			return value.s == o.value.u;
		}
		break;
	case double_long::dl_unsign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.u == o.value.d;
		case double_long::dl_sign:
			return value.u == o.value.s;
		case double_long::dl_unsign:
			return value.u == o.value.u;
		}
	}
	return type == o.type;
}

ULONG64 DoubleLong::ToUnsignedLong()const
{
	ULONG64 ret;

	switch (type)
	{
	case double_long::dl_double:
		memcpy_s(&ret, sizeof(ret), &value.d, sizeof(value.d));
		break;
	case double_long::dl_sign:
		memcpy_s(&ret, sizeof(ret), &value.s, sizeof(value.s));
		break;
	case double_long::dl_unsign:
		ret = value.u;
		break;
	default:
		ret = 0ULL;
	}

	return ret;
}

ULONG64 DoubleLong::GetBitAnd(const DoubleLong& o)
{
	return ToUnsignedLong() & o.ToUnsignedLong();
}

ULONG64 DoubleLong::GetBitOr(const DoubleLong& o)
{
	return ToUnsignedLong() | o.ToUnsignedLong();
}

ULONG64 DoubleLong::GetBitXor(const DoubleLong& o)
{
	return ToUnsignedLong() ^ o.ToUnsignedLong();
}

DoubleLong DoubleLong::GetValueFromPrimitive(TrecPointer<TVariable> var)
{
	if (!var.Get() || var->GetVarType() != var_type::primitive)
		return DoubleLong();

	auto pVar = TrecPointerKey::ConvertPointer<TVariable, TPrimitiveVariable>(var);

	double f_value = 0.0;
	ULONG64 u_value = 0ULL;
	LONG64 s_value = 0LL;

	switch (pVar->GetVType())
	{
	case (TPrimitiveVariable::type_bool):
		return DoubleLong(1ULL);

	case (0b00110010):						// Indicates a four-byte float
		u_value = pVar->Get4Value();
		memcpy_s(&f_value, sizeof(f_value), &u_value, sizeof(u_value));
		return DoubleLong(f_value);

	case (0b01000010):						// Indicates an eight-byte float
		u_value = pVar->Get8Value();
		memcpy_s(&f_value, sizeof(f_value), &u_value, sizeof(u_value));
		return DoubleLong(f_value);

	case (0b00010000):						// Indicates a 1-byte int
	case (0b00100000):						// Indicates a 2-byte int
	case (0b00110000):						// Indicates a 4-byte int
	case (0b01000000):						// Indicates an 8-byte int
		u_value = pVar->Get8Value();
		memcpy_s(&s_value, sizeof(s_value), &u_value, sizeof(u_value));
		return DoubleLong(s_value);

	case (0b00011000):						// Indicates a 1-byte uint
	case (0b00101000):						// Indicates a 2-byte uint
	case (0b00111000):						// Indicates a 4-byte uint
	case (0b01001000):						// Indicates an 8-byte uint
		u_value = pVar->Get8Value();
		return DoubleLong(static_cast<ULONG64>(u_value));


	}
	return DoubleLong();
}

DoubleLong DoubleLong::GetValueFromString(const TString& str)
{
	DoubleLong ret;

	double f = 0.0;
	UINT u = 0;
	int i = 0;

	if (TString::ConvertStringToUint(str, u))
	{
		ret.type = double_long::dl_unsign;
		ret.value.u = u;
	}
	else if (!str.ConvertToInt(i))
	{
		ret.type = double_long::dl_sign;
		ret.value.s = i;
	}
	else if (!str.ConvertToDouble(f))
	{
		ret.type = double_long::dl_double;
		ret.value.d = f;
	}
	return ret;
}
TString DoubleLong::ToString()
{
	TString ret;
	switch (type)
	{
	case double_long::dl_double:
		ret.Format(L"%f", value.d);
		break;
	case double_long::dl_sign:
		ret.Format(L"%d", value.s);
		break;
	case double_long::dl_unsign:
		ret.Format(L"%u", value.u);
	}
	return ret;
}


bool DoubleLong::operator<=(const DoubleLong& o)
{
	switch (type)
	{
	case double_long::dl_double:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.d <= o.value.d;
		case double_long::dl_sign:
			return value.d <= o.value.s;
		case double_long::dl_unsign:
			return value.d <= o.value.u;
		}
		break;
	case double_long::dl_sign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.s <= o.value.d;
		case double_long::dl_sign:
			return value.s <= o.value.s;
		case double_long::dl_unsign:
			return value.s <= o.value.u;
		}
		break;
	case double_long::dl_unsign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.u <= o.value.d;
		case double_long::dl_sign:
			return value.u <= o.value.s;
		case double_long::dl_unsign:
			return value.u <= o.value.u;
		}
	}
	return false;
}

bool DoubleLong::operator>=(const DoubleLong& o)
{
	switch (type)
	{
	case double_long::dl_double:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.d >= o.value.d;
		case double_long::dl_sign:
			return value.d >= o.value.s;
		case double_long::dl_unsign:
			return value.d >= o.value.u;
		}
		break;
	case double_long::dl_sign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.s >= o.value.d;
		case double_long::dl_sign:
			return value.s >= o.value.s;
		case double_long::dl_unsign:
			return value.s >= o.value.u;
		}
		break;
	case double_long::dl_unsign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.u >= o.value.d;
		case double_long::dl_sign:
			return value.u >= o.value.s;
		case double_long::dl_unsign:
			return value.u >= o.value.u;
		}
	}
	return false;
}

bool DoubleLong::operator>(const DoubleLong& o)
{
	switch (type)
	{
	case double_long::dl_double:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.d > o.value.d;
		case double_long::dl_sign:
			return value.d > o.value.s;
		case double_long::dl_unsign:
			return value.d > o.value.u;
		}
		break;
	case double_long::dl_sign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.s > o.value.d;
		case double_long::dl_sign:
			return value.s > o.value.s;
		case double_long::dl_unsign:
			return value.s > o.value.u;
		}
		break;
	case double_long::dl_unsign:
		switch (o.type)
		{
		case double_long::dl_double:
			return value.u > o.value.d;
		case double_long::dl_sign:
			return value.u > o.value.s;
		case double_long::dl_unsign:
			return value.u > o.value.u;
		}
	}
	return false;
}


DoubleLong Add(const DoubleLong& v1, const DoubleLong& v2)
{
	if (v1.type == double_long::dl_double)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(v1.value.d + v2.value.d);
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<double>(v1.value.d + v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<double>(v1.value.d + v2.value.u));
		}
	}
	else if (v1.type == double_long::dl_sign)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.s + v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong((v1.value.s + v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<LONG64>(v1.value.s + v2.value.u));
		}
	}
	else
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.u + v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<LONG64>(v1.value.u + v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<ULONG64>(v1.value.u + v2.value.u));
		}
	}
	return DoubleLong();
}

DoubleLong Subtract(const DoubleLong& v1, const DoubleLong& v2)
{
	if (v1.type == double_long::dl_double)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(v1.value.d - v2.value.d);
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<double>(v1.value.d - v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<double>(v1.value.d - v2.value.u));
		}
	}
	else if (v1.type == double_long::dl_sign)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(static_cast<double>(v1.value.s) - v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong((v1.value.s - v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<LONG64>(v1.value.s - v2.value.u));
		}
	}
	else
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.u - v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<LONG64>(v1.value.u - v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<ULONG64>(v1.value.u - v2.value.u));
		}
	}
	return DoubleLong();
}

DoubleLong Multiply(const DoubleLong& v1, const DoubleLong& v2)
{
	if (v1.type == double_long::dl_double)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(v1.value.d * v2.value.d);
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<double>(v1.value.d * v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<double>(v1.value.d * v2.value.u));
		}
	}
	else if (v1.type == double_long::dl_sign)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.s * v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong((v1.value.s * v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<LONG64>(v1.value.s * v2.value.u));
		}
	}
	else
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.u * v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<LONG64>(v1.value.u * v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<ULONG64>(v1.value.u * v2.value.u));
		}
	}
	return DoubleLong();
}

DoubleLong Divide(const DoubleLong& v1, const DoubleLong& v2)
{
	if (v1.type == double_long::dl_double)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(v1.value.d / v2.value.d);
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<double>(v1.value.d / v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<double>(v1.value.d / v2.value.u));
		}
	}
	else if (v1.type == double_long::dl_sign)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.s / v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong((v1.value.s / v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<LONG64>(v1.value.s / v2.value.u));
		}
	}
	else
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(static_cast<double>(v1.value.u / v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<LONG64>(v1.value.u / v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<ULONG64>(v1.value.u / v2.value.u));
		}
	}
	return DoubleLong();
}

DoubleLong ModDivide(const DoubleLong& v1, const DoubleLong& v2)
{
	if (v1.type == double_long::dl_double)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(fmod(v1.value.d, v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(fmod(v1.value.d, v2.value.s));
		}
		else
		{
			return DoubleLong(fmod(v1.value.d, v2.value.s));
		}
	}
	else if (v1.type == double_long::dl_sign)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(fmod(v1.value.d, v2.value.s));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong((v1.value.s % v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<LONG64>(v1.value.s % v2.value.u));
		}
	}
	else
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(fmod(v1.value.d, v2.value.s));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(static_cast<LONG64>(v1.value.u % v2.value.s));
		}
		else
		{
			return DoubleLong(static_cast<ULONG64>(v1.value.u % v2.value.u));
		}
	}
	return DoubleLong();
}

DoubleLong Exponent(const DoubleLong& v1, const DoubleLong& v2)
{
	if (v1.type == double_long::dl_double)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(pow(v1.value.d, v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(pow(v1.value.d, v2.value.s));
		}
		else
		{
			return DoubleLong(pow(v1.value.d, v2.value.u));
		}
	}
	else if (v1.type == double_long::dl_sign)
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(pow(v1.value.s, v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(pow(v1.value.s, v2.value.s));
		}
		else
		{
			return DoubleLong(pow(v1.value.s, v2.value.u));
		}
	}
	else
	{
		if (v2.type == double_long::dl_double)
		{
			return DoubleLong(pow(v1.value.u, v2.value.d));
		}
		else if (v2.type == double_long::dl_sign)
		{
			return DoubleLong(pow(v1.value.u, v2.value.s));
		}
		else
		{
			return DoubleLong(pow(v1.value.u, v2.value.u));
		}
	}
	return DoubleLong();
}

TString GetStringFromPrimitive(TrecPointer<TVariable> var)
{
	if (!var.Get() || var->GetVarType() != var_type::primitive)
		return TString();
	TString strValue;


	auto pVar = TrecPointerKey::ConvertPointer<TVariable, TPrimitiveVariable>(var);

	UINT v1Type = pVar->GetVType();

	if (v1Type == (TPrimitiveVariable::type_char & TPrimitiveVariable::type_one))
		strValue.Set(static_cast<char>(var->Get4Value()));
	else if (v1Type == (TPrimitiveVariable::type_char & TPrimitiveVariable::type_two))
		strValue.Set(static_cast<WCHAR>(var->Get4Value()));
	else if (v1Type == TPrimitiveVariable::type_bool)
	{
		if (pVar->Get8Value())
			strValue.Set(L"true");
		else
			strValue.Set(L"false");
	}
	else
	{
		auto value = DoubleLong::GetValueFromPrimitive(var);
		switch (value.type)
		{
		case double_long::dl_double:
			strValue.Format(L"%f", value.value.d);
			break;
		case double_long::dl_sign:
			strValue.Format(L"%d", value.value.s);
			break;
		case double_long::dl_unsign:
			strValue.Format(L"%u", value.value.u);
		}
	}
	return strValue;
}