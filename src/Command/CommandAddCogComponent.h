#pragma once

#include "Core/ITransform.h"
#include "Cogs/CogResourceId.h"

namespace xg::command
{
	struct AddCogComponent
	{
		xg::CogResourceId m_CogId;
		xc::ITransform m_Transform;
	};
}
