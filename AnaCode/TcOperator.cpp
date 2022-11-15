#include "TcOperator.h"
#include "TPrimitiveVariable.h"
#include <TContainerVariable.h>

using OpMap = struct OpMap {
	tc_int_op op;
	TString opName;
};

OpMap opMap[] = {
	// Artihmetic
	{tc_int_op::add, L"Add"},
	{tc_int_op::add_assign, L"AddAssign"},
	{tc_int_op::sub, L"Sub"},
	{tc_int_op::sub_assign, L"SubAssign"},
	{tc_int_op::mult, L"Mult"},
	{tc_int_op::mul_assign, L"MultAssign"},
	{tc_int_op::div, L"Div"},
	{tc_int_op::div_assign, L"DivAssign"},
	{tc_int_op::mod, L"Mod"},
	{tc_int_op::mod_assign, L"ModAssign"},
	{tc_int_op::pow, L"Pow"},
	{tc_int_op::pow_assign, L"PowAssign"},
	// Logical Ops
	{tc_int_op::and_l, L"LogicalAnd"},
	{tc_int_op::or_l, L"LogicalOr"},
	{tc_int_op::xor_l, L"LogicalXor"},
	{tc_int_op::not_l, L"LogicalNot"},
	{tc_int_op::and_assign, L"LogicalAndAssign"},
	{tc_int_op::or_assign, L"LogicalOrAssign"},
	{tc_int_op::xor_assign, L"LogicalXorAssign"},
	// Bitwise Ops
	{tc_int_op::and_b, L"BitwiseAnd"},
	{tc_int_op::or_b, L"BitwiseOr"},
	{tc_int_op::xor_b, L"BitwiseXor"},
	{tc_int_op::left_b, L"BitwiseLeft"},
	{tc_int_op::right_b, L"BitwiseRight"},
	{tc_int_op::b_and_assign, L"BitwiseAnd"},
	{tc_int_op::b_or_assign, L"BitwiseOr"},
	{tc_int_op::b_xor_assign, L"BitwiseXor"},
	{tc_int_op::b_left_assign, L"BitwiseLeft"},
	{tc_int_op::b_right_assign, L"BitwiseRight"},
	// Equality
	{tc_int_op::eq, L"Equals"},
	{tc_int_op::eq_t, L"TypeEquals"},
	{tc_int_op::gt, L"GreaterThan"},
	{tc_int_op::gte, L"GreaterThanEquals"},
	{tc_int_op::lt, L"LessThan"},
	{tc_int_op::lte, L"LessThanEquals"},
	{tc_int_op::not_e, L"NotEquals"},
	{tc_int_op::not_e_t, L"NotTypeEquals"},
	// Other
	{tc_int_op::basic_exp, L"BasicExpansion"},
	{tc_int_op::point_exp, L"PointExpansion"},
	{tc_int_op::reg_assign, L"RegAssign"},
	{tc_int_op::conditional, L"Conditional"},
	{tc_int_op::separator, L"Separator"}

};

tc_int_op GetOpFromString(const TString& opName)
{
	for (UINT Rust = 0; Rust < ARRAYSIZE(opMap); Rust++)
	{
		if (!opName.Compare(opMap[Rust].opName))
			return opMap[Rust].op;
	}
	return tc_int_op::none;
}

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

void TcOperatorGroup::SetLeftToRight(bool b)
{
	this->leftToRight = b;
}

void TcOperatorGroup::AppendOperator(tc_int_op op)
{
	this->operators.push_back(op);
}

bool TcOperatorGroup::IsLeftToRight()
{
	return this->leftToRight;
}

bool TcOperatorGroup::ContainsOperator(tc_int_op op)
{
	for (UINT Rust = 0; Rust < operators.Size(); Rust++)
	{
		if (operators[Rust] == op)
			return true;
	}
	return false;
}

bool TcOperatorGroupList::SetVariable(const TString& name, TrecPointer<TVariable> var)
{
	TrecPointer<TArrayVariable> groups = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(var);
	if(!groups.Get())
		return false;
	TrecPointer<TVariable> element;

	bool works = true;

	for (UINT Rust = 0; works && groups->At(element, Rust); Rust++)
	{
		TrecPointer<TJsonVariable> group = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(element);
		if (!group.Get())
		{
			works = false;
			break;
		}
		TrecPointer<TVariable> leftToRight;
		TrecPointer<TVariable> ops;
		if (!group->RetrieveField(L"LeftToRight", leftToRight) && !group->RetrieveField(L"OpList", ops))
		{
			works = false;
			break;
		}

		TrecPointer<TPrimitiveVariable> actLeftToRight = TrecPointerKey::ConvertPointer<TVariable, TPrimitiveVariable>(leftToRight);
		TrecPointer<TArrayVariable> actOps = TrecPointerKey::ConvertPointer<TVariable, TArrayVariable>(ops);

		if (!actLeftToRight.Get() || !actOps.Get())
		{
			works = false;
			break;
		}

		TDataArray<tc_int_op> opsList;

		TrecPointer<TVariable> opName;

		for (UINT C = 0; actOps->At(opName, C); C++)
		{
			TrecPointer<TStringVariable> strOpName = TrecPointerKey::ConvertPointer<TVariable, TStringVariable>(opName);
			if (!strOpName.Get())
			{
				works = false;
				break;
			}

			tc_int_op curOp = GetOpFromString(strOpName->GetString());

			if (curOp == tc_int_op::none)
			{
				works = false;
				break;
			}

			opsList.push_back(curOp);
		}

		TcOperatorGroup newGroup;
		newGroup.SetLeftToRight(actLeftToRight->Get4Value() > 0);
		for (UINT C = 0; C < opsList.Size(); C++)
		{
			newGroup.AppendOperator(opsList[C]);
		}

		this->opList.push_back(newGroup);
	}
	if (!works)
		opList.RemoveAll();

	return works;
}

UINT TcOperatorGroupList::GetGroupCount()
{
	return this->opList.Size();
}

bool TcOperatorGroupList::GetGroup(TcOperatorGroup& group, UINT Rust)
{
	if(Rust >= opList.Size())
		return false;
	group = opList[Rust];
	return true;
}

void TcOperator::IsTruthful(TrecPointer<TVariable> v1, bool& worked, bool& truthful, ReturnObject& ret)
{
	IsTruthful(v1, ret);
	worked = ret.returnCode == 0;

	truthful = ret.errorObject->Get4Value() != 0;
}

void TcOperator::LogicalOp(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, tc_int_op op, ReturnObject& obj)
{
	bool b1, b2, worked;
	IsTruthful(v1, worked, b1, obj);
	if (!worked)
		return;
	IsTruthful(v2, worked, b2, obj);
	if (!worked)
		return;

	bool doCondense = CondenseToBoolean();

	switch (op)
	{
	case tc_int_op::and_l:
	case tc_int_op::and_assign:
		obj.errorObject = doCondense ? TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(b1 && b2) : (!b1 ? v1 : v2);
		return;
	case tc_int_op::or_l:
	case tc_int_op::or_assign:
		obj.errorObject = doCondense ? TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(b1 || b2) : (b1 ? v1 : v2);
		return;
	case tc_int_op::xor_l:
	case tc_int_op::xor_assign:
		obj.errorObject = doCondense ? TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(b1 ^ b2) : ((b1 && b2) ? TPrimitiveVariable::GetFalse() : (b1 ? v1 : v2));
		return;
	default:
		obj.returnCode = obj.ERR_INTERNAL;
		obj.errorMessage.Set("Unexpected Operator passed into Logical Operator Processor!");
	}
}

void TcOperator::BitwiseOp(TrecPointer<TVariable> v1, TrecPointer<TVariable> v2, tc_int_op op, ReturnObject& obj)
{
	DoubleLong dl1 = DoubleLong::GetValueFromPrimitive(v1);
	DoubleLong dl2 = DoubleLong::GetValueFromPrimitive(v2);

	if (dl1.type == double_long::dl_invalid || dl2.type == double_long::dl_invalid)
	{
		obj.returnCode = obj.ERR_IMPROPER_TYPE;
		obj.errorMessage.Set(L"Bitwise Operations need to be performed on numeric types!");
		return;
	}

	ULONG64 baseValue, shiftValue;

	switch (op)
	{
	case tc_int_op::and_b:
	case tc_int_op::b_and_assign:
		obj.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(dl1.GetBitAnd(dl2));
		return;
	case tc_int_op::or_b:
	case tc_int_op::b_or_assign:
		obj.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(dl1.GetBitOr(dl2));
		return;
	case tc_int_op::xor_b:
	case tc_int_op::b_xor_assign:
		obj.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(dl1.GetBitXor(dl2));
		return;
	case tc_int_op::left_b:
	case tc_int_op::b_left_assign:
		baseValue = dl1.ToUnsignedLong();
		shiftValue = dl2.ToUnsignedLong();
		obj.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(baseValue << shiftValue);
		return;
	case tc_int_op::right_b:
	case tc_int_op::b_right_assign:
		baseValue = dl1.ToUnsignedLong();
		shiftValue = dl2.ToUnsignedLong();
		obj.errorObject = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, TPrimitiveVariable>(baseValue >> shiftValue);
		return;
	default:
		obj.returnCode = obj.ERR_INTERNAL;
		obj.errorMessage.Set("Unexpected Operator passed into Bitwise Operator Processor!");
	}
}
